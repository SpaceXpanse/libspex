// Copyright (C) 2020-2023 The Xaya developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "config.h"

#include "logic.hpp"
#include "pending.hpp"
#include "rpcserver.hpp"

#include "xayagame/defaultmain.hpp"
#include "xayagame/sqliteproc.hpp"

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <cstdlib>
#include <iostream>
#include <memory>

namespace
{

DEFINE_string (xaya_rpc_url, "",
               "URL at which Xaya Core's JSON-RPC interface is available");
DEFINE_int32 (xaya_rpc_protocol, 1,
              "JSON-RPC version for connecting to Xaya Core");
DEFINE_bool (xaya_rpc_wait, false,
             "whether to wait on startup for Xaya Core to be available");

DEFINE_int32 (game_rpc_port, 0,
              "the port at which the GSP JSON-RPC server will be started"
              " (if non-zero)");
DEFINE_bool (game_rpc_listen_locally, true,
             "whether the GSP's JSON-RPC server should listen locally");

DEFINE_int32 (enable_pruning, -1,
              "if non-negative (including zero), old undo data will be pruned"
              " and only as many blocks as specified will be kept");
DEFINE_int32 (statehash_interval, 0,
              "if set to a positive number N, hash the game state"
              " every N blocks");
DEFINE_string (target_block, "",
               "if set to a block hash, sync to this block and then stop");

DEFINE_string (datadir, "",
               "base data directory for state data"
               " (will be extended by 'nf' and the chain)");

DEFINE_bool (pending_moves, true,
             "whether or not pending moves should be tracked");

class NFInstanceFactory : public xaya::CustomisedInstanceFactory
{

private:

  /**
   * Reference to the NonFungibleLogic instance.  This is needed to construct
   * the RPC server.
   */
  nf::NonFungibleLogic& logic;

  /** The game-state hasher used, if hashing is enabled.  */
  std::unique_ptr<xaya::SQLiteHasher> hasher;

public:

  explicit NFInstanceFactory (nf::NonFungibleLogic& l)
    : logic(l)
  {
    if (FLAGS_statehash_interval > 0)
      {
        hasher = std::make_unique<xaya::SQLiteHasher> ();
        hasher->SetInterval (FLAGS_statehash_interval);
        logic.AddProcessor (*hasher);
      }
  }

  std::unique_ptr<xaya::RpcServerInterface>
  BuildRpcServer (xaya::Game& game,
                  jsonrpc::AbstractServerConnector& conn) override
  {
    std::unique_ptr<xaya::RpcServerInterface> res;
    auto* h = hasher.get ();
    res.reset (new xaya::WrappedRpcServer<nf::RpcServer> (
        game, logic, h, conn));
    return res;
  }

  std::vector<std::unique_ptr<xaya::GameComponent>>
  BuildGameComponents (xaya::Game& g)
  {
    /* We don't actually use any custom game components, but we can use this
       method to set up the target block on Game.  */
    if (!FLAGS_target_block.empty ())
      {
        xaya::uint256 hash;
        CHECK (hash.FromHex (FLAGS_target_block))
            << "Invalid block hash set as target: " << FLAGS_target_block;
        g.SetTargetBlock (hash);
      }

    return xaya::CustomisedInstanceFactory::BuildGameComponents (g);
  }

};

} // anonymous namespace

int
main (int argc, char** argv)
{
  google::InitGoogleLogging (argv[0]);

  gflags::SetUsageMessage ("Run nonfungible GSP");
  gflags::SetVersionString (PACKAGE_VERSION);
  gflags::ParseCommandLineFlags (&argc, &argv, true);

  if (FLAGS_xaya_rpc_url.empty ())
    {
      std::cerr << "Error: --xaya_rpc_url must be set" << std::endl;
      return EXIT_FAILURE;
    }
  if (FLAGS_datadir.empty ())
    {
      std::cerr << "Error: --datadir must be specified" << std::endl;
      return EXIT_FAILURE;
    }

  xaya::GameDaemonConfiguration config;
  config.XayaRpcUrl = FLAGS_xaya_rpc_url;
  config.XayaJsonRpcProtocol = FLAGS_xaya_rpc_protocol;
  config.XayaRpcWait = FLAGS_xaya_rpc_wait;
  if (FLAGS_game_rpc_port != 0)
    {
      config.GameRpcServer = xaya::RpcServerType::HTTP;
      config.GameRpcPort = FLAGS_game_rpc_port;
      config.GameRpcListenLocally = FLAGS_game_rpc_listen_locally;
    }
  config.EnablePruning = FLAGS_enable_pruning;
  config.DataDirectory = FLAGS_datadir;

  nf::NonFungibleLogic logic;

  NFInstanceFactory fact(logic);
  config.InstanceFactory = &fact;

  nf::PendingMoves pending(logic);
  if (FLAGS_pending_moves)
    config.PendingMoves = &pending;

  return xaya::SQLiteMain (config, "nf", logic);
}
