// Copyright (C) 2020-2023 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NONFUNGIBLE_RPCSERVER_HPP
#define NONFUNGIBLE_RPCSERVER_HPP

#include "logic.hpp"
#include "rpc-stubs/nfrpcserverstub.h"

#include "spacexpansegame/game.hpp"
#include "spacexpansegame/sqliteproc.hpp"

#include <json/json.h>
#include <jsonrpccpp/server.h>

#include <string>

namespace nf
{

/**
 * RPC interface for nonfungibled.
 */
class RpcServer : public NFRpcServerStub
{

private:

  /** The underlying Game instance that manages everything.  */
  spacexpanse::Game& game;

  /** The NF logic instance for the SQLite database.  */
  NonFungibleLogic& logic;

  /** The state hasher, if any.  */
  spacexpanse::SQLiteHasher* hasher;

public:

  /**
   * Builds the RPC server bound to the running game instance.
   */
  explicit RpcServer (spacexpanse::Game& g, NonFungibleLogic& l, spacexpanse::SQLiteHasher* h,
                      jsonrpc::AbstractServerConnector& conn)
    : NFRpcServerStub(conn), game(g), logic(l), hasher(h)
  {}

  /** Requests a graceful daemon shutdown. */
  void stop () override;

  /** Returns the currently confirmed game state JSON. */
  Json::Value getcurrentstate () override;
  /** Returns the null/initial state JSON. */
  Json::Value getnullstate () override;
  /** Returns the currently tracked pending state JSON. */
  Json::Value getpendingstate () override;

  /** Computes and returns a full hash of the current SQLite state. */
  Json::Value hashcurrentstate () override;
  /** Returns a precomputed state hash for a specific block if available. */
  Json::Value getstatehash (const std::string& block) override;

  /** Sets or clears the target block for synchronisation. */
  void settargetblock (const std::string& block) override;

  /** Blocks until the confirmed chain tip differs from knownBlock. */
  std::string waitforchange (const std::string& knownBlock) override;
  /** Blocks until pending-state version differs from knownVersion. */
  Json::Value waitforpendingchange (int knownVersion) override;

  /** Lists all known assets. */
  Json::Value listassets () override;
  /** Returns details (metadata, supply, balances) for one asset. */
  Json::Value getassetdetails (const Json::Value& asset) override;
  /** Returns balance for one (asset, name) tuple. */
  Json::Value getbalance (const Json::Value& asset,
                          const std::string& name) override;
  /** Returns all balances held by a user. */
  Json::Value getuserbalances (const std::string& name) override;

};

} // namespace nf

#endif // NONFUNGIBLE_RPCSERVER_HPP
