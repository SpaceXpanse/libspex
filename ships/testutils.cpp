// Copyright (C) 2019-2020 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "testutils.hpp"

#include "coord.hpp"

#include <glog/logging.h>

#include <sstream>

namespace ships
{

Json::Value
ParseJson (const std::string& str)
{
  /* Minimal helper used by tests to decode inline JSON literals.  */
  std::istringstream in(str);

  Json::Value res;
  in >> res;
  CHECK (in);

  return res;
}

InMemoryLogicFixture::InMemoryLogicFixture ()
  : game(verifier)
{
  /* Build fully initialised in-memory game instance for deterministic tests.  */
  game.Initialise (":memory:");
  game.InitialiseGameContext (spacexpanse::Chain::MAIN, "xs", nullptr);
  game.GetStorage ().Initialise ();
  /* The initialisation above already sets up the database schema.  */
}

spacexpanse::SQLiteDatabase&
InMemoryLogicFixture::GetDb ()
{
  return game.GetDatabaseForTesting ();
}

const ShipsBoardRules&
InMemoryLogicFixture::GetBoardRules () const
{
  /* Tests rely on concrete ships rules beyond generic BoardRules interface.  */
  return dynamic_cast<const ShipsBoardRules&> (game.GetBoardRules ());
}

} // namespace ships
