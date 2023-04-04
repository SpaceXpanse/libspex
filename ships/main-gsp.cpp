// Copyright (C) 2019-2021 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "config.h"

#include "logic.hpp"

#include <sidechannel/gsprpc.hpp>
#include <spacexpansegame/defaultmain.hpp>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <google/protobuf/stubs/common.h>

#include <cstdlib>
#include <iostream>

namespace
{

DEFINE_string (spacexpanse_rpc_url, "",
               "URL at which SpaceXpanse Core's JSON-RPC interface is available");
DEFINE_int32 (spacexpanse_rpc_protocol, 1,
              "JSON-RPC version for connecting to SpaceXpanse Core");
DEFINE_bool (spacexpanse_rpc_wait, false,
             "whether to wait on startup for SpaceXpanse Core to be available");

DEFINE_int32 (game_rpc_port, 0,
              "the port at which the game daemon's JSON-RPC server will be"
              " started (if non-zero)");
DEFINE_bool (game_rpc_listen_locally, true,
             "whether the game daemon's JSON-RPC server should listen locally");

DEFINE_int32 (enable_pruning, -1,
              "if non-negative (including zero), enable pruning of old undo"
              " data and keep as many blocks as specified by the value");

DEFINE_string (datadir, "",
               "base data directory for game data (will be extended by the"
               " game ID and chain)");

DEFINE_bool (pending_moves, true,
             "whether or not pending moves should be tracked");

} // anonymous namespace

int
main (int argc, char** argv)
{
  google::InitGoogleLogging (argv[0]);
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  gflags::SetUsageMessage ("Run SpaceXpanseships game daemon");
  gflags::SetVersionString (PACKAGE_VERSION);
  gflags::ParseCommandLineFlags (&argc, &argv, true);

  if (FLAGS_spacexpanse_rpc_url.empty ())
    {
      std::cerr << "Error: --spacexpanse_rpc_url must be set" << std::endl;
      return EXIT_FAILURE;
    }
  if (FLAGS_datadir.empty ())
    {
      std::cerr << "Error: --datadir must be specified" << std::endl;
      return EXIT_FAILURE;
    }

  spacexpanse::GameDaemonConfiguration config;
  config.SpaceXpanseRpcUrl = FLAGS_spacexpanse_rpc_url;
  config.SpaceXpanseJsonRpcProtocol = FLAGS_spacexpanse_rpc_protocol;
  config.SpaceXpanseRpcWait = FLAGS_spacexpanse_rpc_wait;
  if (FLAGS_game_rpc_port != 0)
    {
      config.GameRpcServer = spacexpanse::RpcServerType::HTTP;
      config.GameRpcPort = FLAGS_game_rpc_port;
      config.GameRpcListenLocally = FLAGS_game_rpc_listen_locally;
    }
  config.EnablePruning = FLAGS_enable_pruning;
  config.DataDirectory = FLAGS_datadir;

  ships::ShipsLogic rules;
  spacexpanse::ChannelGspInstanceFactory instanceFact(rules);
  config.InstanceFactory = &instanceFact;

  ships::ShipsPending pending(rules);
  if (FLAGS_pending_moves)
    config.PendingMoves = &pending;

  const int res = spacexpanse::SQLiteMain (config, "xs", rules);
  google::protobuf::ShutdownProtobufLibrary ();
  return res;
}
