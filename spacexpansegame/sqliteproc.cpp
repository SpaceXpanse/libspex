// Copyright (C) 2022 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "sqliteproc.hpp"

#include "sqliteintro.hpp"

#include <spacexpanseutil/hash.hpp>

#include <glog/logging.h>

namespace spacexpanse
{

/* ************************************************************************** */

bool
SQLiteProcessor::ShouldRun (const Json::Value& blockData) const
{
  if (blockInterval == 0)
    return false;

  CHECK (blockData.isObject ());
  const auto& heightVal = blockData["height"];
  CHECK (heightVal.isUInt64 ());
  const uint64_t height = heightVal.asUInt64 ();

  return (height % blockInterval) == blockModulo;
}

void
SQLiteProcessor::SetupSchema (SQLiteDatabase& db)
{}

void
SQLiteProcessor::Finish ()
{
  /* Nothing needs to be done for now while processing runs synchronously.  */
}

void
SQLiteProcessor::Process (const Json::Value& blockData, SQLiteDatabase& db)
{
  if (!ShouldRun (blockData))
    return;

  /* TODO:  Actually make Compute() run on a snapshot asynchronously,
     rather than blocking the entire GSP process for a long-running computation
     (like hashing of a large game state).

     For now we just run it synchronously here for simplicity.  */

  Compute (blockData, db);

  db.Prepare ("SAVEPOINT `spacexpansegame-processor`").Execute ();
  Store (db);
  db.Prepare ("RELEASE `spacexpansegame-processor`").Execute ();
}

void
SQLiteProcessor::SetInterval (const uint64_t intv, const uint64_t modulo)
{
  blockInterval = intv;
  blockModulo = modulo;
}

/* ************************************************************************** */

void
SQLiteHasher::SetupSchema (SQLiteDatabase& db)
{
  SQLiteProcessor::SetupSchema (db);
  db.Execute (R"(
    CREATE TABLE IF NOT EXISTS `spacexpansegame_statehashes`
        (`block` BLOB PRIMARY KEY,
         `hash` BLOB NOT NULL);
  )");
} 

std::set<std::string>
SQLiteHasher::GetTables (const SQLiteDatabase& db)
{
  return GetSqliteTables (db);
}

void
SQLiteHasher::Compute (const Json::Value& blockData, const SQLiteDatabase& db)
{
  const auto hashVal = blockData["hash"];
  CHECK (hashVal.isString ());
  CHECK (block.FromHex (hashVal.asString ()));

  VLOG (1) << "Computing game-state hash for block " << block.ToHex ();
  SHA256 hasher;
  WriteTables (hasher, db, GetTables (db));
  hash = hasher.Finalise ();
}

void
SQLiteHasher::Store (SQLiteDatabase& db)
{
  /* First check that if a hash exists already, it matches what we computed.
     Otherwise there is some kind of serious bug.  */
  uint256 existing;
  if (GetHash (db, block, existing))
    CHECK (existing == hash)
        << "Already stored game-state differs from computed for block "
        << block.ToHex ();

  auto stmt = db.Prepare (R"(
    INSERT OR IGNORE INTO `spacexpansegame_statehashes`
      (`block`, `hash`) VALUES (?1, ?2)
  )");
  stmt.Bind (1, block);
  stmt.Bind (2, hash);
  stmt.Execute ();
}

bool
SQLiteHasher::GetHash (const SQLiteDatabase& db, const uint256& block,
                       uint256& hash) const
{
  auto stmt = db.PrepareRo (R"(
    SELECT `hash`
      FROM `spacexpansegame_statehashes`
      WHERE `block` = ?1
  )");
  stmt.Bind (1, block);

  if (!stmt.Step ())
    return false;

  hash = stmt.Get<uint256> (0);
  CHECK (!stmt.Step ());
  return true;
}

/* ************************************************************************** */

} // namespace spacexpanse
