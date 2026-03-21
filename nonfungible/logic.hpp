// Copyright (C) 2020-2022 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NONFUNGIBLE_LOGIC_HPP
#define NONFUNGIBLE_LOGIC_HPP

#include "statejson.hpp"

#include <spacexpansegame/sqlitegame.hpp>
#include <spacexpansegame/sqlitestorage.hpp>

#include <json/json.h>

#include <functional>
#include <string>

namespace nf
{

/**
 * The game logic implementation for the non-fungible game-state processor.
 */
class NonFungibleLogic : public spacexpanse::SQLiteGame
{

protected:

  /**
   * Creates (or validates) the SQLite schema used for confirmed game state.
   */
  void SetupSchema (spacexpanse::SQLiteDatabase& db) override;

  /**
   * Provides the chain-specific anchor block for deterministic initial sync.
   */
  void GetInitialStateBlock (unsigned& height,
                             std::string& hashHex) const override;

  /**
   * Initialises an empty state database at the anchor block.
   */
  void InitialiseState (spacexpanse::SQLiteDatabase& db) override;

  /**
   * Applies all block moves to the confirmed SQLite state.
   */
  void UpdateState (spacexpanse::SQLiteDatabase& db,
                    const Json::Value& blockData) override;

  /**
   * Serialises the full confirmed state into JSON.
   */
  Json::Value GetStateAsJson (const spacexpanse::SQLiteDatabase& db) override;

public:

  /**
   * Type for a callback that extracts custom JSON from the game state
   * (through a StateJsonExtractor instance).
   */
  using StateCallback
      = std::function<Json::Value (const StateJsonExtractor& ext)>;

  NonFungibleLogic () = default;

  NonFungibleLogic (const NonFungibleLogic&) = delete;
  void operator= (const NonFungibleLogic&) = delete;

  /**
   * Extracts some custom JSON from the current game-state database, using
   * the provided extractor callback, which can then operate through a
   * StateJsonExtractor instance.
   */
  Json::Value GetCustomStateData (spacexpanse::Game& game, const StateCallback& cb);

  /* Expose the super-class variant as well, which is used for
     state hashing.  */
  using SQLiteGame::GetCustomStateData;

};

} // namespace nf

#endif // NONFUNGIBLE_LOGIC_HPP
