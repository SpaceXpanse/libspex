// Copyright (C) 2018-2021 The SpaceXpanse developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "defaultmain.hpp"

#include "gamerpcserver.hpp"
#include "lmdbstorage.hpp"
#include "sqlitestorage.hpp"

#include "rpc-stubs/spacexpanserpcclient.h"

#include <jsonrpccpp/client/connectors/httpclient.h>
#include <jsonrpccpp/common/exception.h>

#include <glog/logging.h>

#include <experimental/filesystem>

#include <chrono>
#include <cstdlib>
#include <exception>
#include <thread>

namespace spacexpanse
{

std::unique_ptr<RpcServerInterface>
CustomisedInstanceFactory::BuildRpcServer (
    Game& game, jsonrpc::AbstractServerConnector& conn)
{
  std::unique_ptr<RpcServerInterface> res;
  res.reset (new WrappedRpcServer<GameRpcServer> (game, conn));
  return res;
}

std::vector<std::unique_ptr<GameComponent>>
CustomisedInstanceFactory::BuildGameComponents (Game& game)
{
  return {};
}

namespace
{

namespace fs = std::experimental::filesystem;

/**
 * Returns the directory in which data for this game should be stored.
 * The directory is created as needed.
 */
fs::path
GetGameDirectory (const GameDaemonConfiguration& config,
                  const std::string& gameId, const Chain chain)
{
  CHECK (!config.DataDirectory.empty ()) << "DataDirectory has not been set";
  const fs::path gameDir
      = fs::path (config.DataDirectory)
          / fs::path (gameId)
          / fs::path (ChainToString (chain));

  if (fs::is_directory (gameDir))
    LOG (INFO) << "Using existing data directory: " << gameDir;
  else
    {
      LOG (INFO) << "Creating data directory: " << gameDir;
      CHECK (fs::create_directories (gameDir));
    }

  return gameDir;
}

/**
 * Sets up a StorageInterface instance according to the configuration.
 */
std::unique_ptr<StorageInterface>
CreateStorage (const GameDaemonConfiguration& config,
               const std::string& gameId, const Chain chain)
{
  if (config.StorageType == "memory")
    return std::make_unique<MemoryStorage> ();

  const fs::path gameDir = GetGameDirectory (config, gameId, chain);

  if (config.StorageType == "lmdb")
    {
      const fs::path lmdbDir = gameDir / fs::path ("lmdb");
      if (!fs::is_directory (lmdbDir))
        {
          LOG (INFO) << "Creating directory for LMDB environment: " << lmdbDir;
          CHECK (fs::create_directories (lmdbDir));
        }
      return std::make_unique<LMDBStorage> (lmdbDir.string ());
    }

  if (config.StorageType == "sqlite")
    {
      const fs::path dbFile = gameDir / fs::path ("storage.sqlite");
      return std::make_unique<SQLiteStorage> (dbFile.string ());
    }

  LOG (FATAL) << "Invalid storage type selected: " << config.StorageType;
  return nullptr;
}

/**
 * Constructs the server connector for the JSON-RPC server (if any) based
 * on the configuration.
 */
std::unique_ptr<jsonrpc::AbstractServerConnector>
CreateRpcServerConnector (const GameDaemonConfiguration& config)
{
  switch (config.GameRpcServer)
    {
    case RpcServerType::NONE:
      return nullptr;

    case RpcServerType::HTTP:
      {
        CHECK (config.GameRpcPort != 0)
            << "GameRpcPort must be specified for HTTP server type";
        LOG (INFO)
            << "Starting JSON-RPC HTTP server at port " << config.GameRpcPort;
        auto srv = std::make_unique<jsonrpc::HttpServer> (config.GameRpcPort);
        if (config.GameRpcListenLocally)
          srv->BindLocalhost ();
        return srv;
      }
    }

  LOG (FATAL)
      << "Invalid GameRpcServer value: "
      << static_cast<int> (config.GameRpcServer);
}

/**
 * Checks the SpaceXpanse Core version against the minimum and maximum versions
 * defined in the configuration.
 */
void
VerifySpaceXpanseVersion (const GameDaemonConfiguration& config, const unsigned v)
{
  LOG (INFO) << "Connected to SpaceXpanse Core version " << v;
  CHECK_GE (v, config.MinSpaceXpanseVersion) << "SpaceXpanse Core is too old";
  if (config.MaxSpaceXpanseVersion > 0)
    CHECK_LE (v, config.MaxSpaceXpanseVersion) << "SpaceXpanse Core is too new";
}

/**
 * Waits for the SpaceXpanse Core RPC interface to be available on the given
 * client connector.
 */
void
WaitForSpaceXpanse (jsonrpc::IClientConnector& conn, const jsonrpc::clientVersion_t v)
{
  LOG (INFO) << "Waiting for SpaceXpanse to be up...";

  SpaceXpanseRpcClient client(conn, v);
  while (true)
    try
      {
        client.getnetworkinfo ();
        LOG (INFO) << "SpaceXpanse Core is available now";
        break;
      }
    catch (const jsonrpc::JsonRpcException& exc)
      {
        VLOG (1) << exc.what ();
        LOG (INFO) << "Failed to connect to SpaceXpanse Core, waiting...";
        std::this_thread::sleep_for (std::chrono::seconds (1));
      }
}

/**
 * Converts the protocol version as integer (1 or 2) from the
 * config struct to the libjson-rpc-cpp type.
 */
jsonrpc::clientVersion_t
GetClientVersion (const GameDaemonConfiguration& config)
{
  switch (config.SpaceXpanseJsonRpcProtocol)
    {
    case 1:
      return jsonrpc::JSONRPC_CLIENT_V1;
    case 2:
      return jsonrpc::JSONRPC_CLIENT_V2;
    default:
      LOG (FATAL) << "Invalid JSON-RPC version: " << config.SpaceXpanseJsonRpcProtocol;
    }
}

} // anonymous namespace

int
DefaultMain (const GameDaemonConfiguration& config, const std::string& gameId,
             GameLogic& rules)
{
  try
    {
      CustomisedInstanceFactory* instanceFact = config.InstanceFactory;
      std::unique_ptr<CustomisedInstanceFactory> defaultInstanceFact;
      if (instanceFact == nullptr)
        {
          defaultInstanceFact = std::make_unique<CustomisedInstanceFactory> ();
          instanceFact = defaultInstanceFact.get ();
        }

      CHECK (!config.SpaceXpanseRpcUrl.empty ()) << "SpaceXpanseRpcUrl must be configured";
      const std::string jsonRpcUrl(config.SpaceXpanseRpcUrl);
      jsonrpc::HttpClient httpConnector(jsonRpcUrl);

      const auto protocol = GetClientVersion (config);
      if (config.SpaceXpanseRpcWait)
        WaitForSpaceXpanse (httpConnector, protocol);

      auto game = std::make_unique<Game> (gameId);
      game->ConnectRpcClient (httpConnector, protocol);
      VerifySpaceXpanseVersion (config, game->GetSpaceXpanseVersion ());
      CHECK (game->DetectZmqEndpoint ());

      std::unique_ptr<StorageInterface> storage
          = CreateStorage (config, gameId, game->GetChain ());
      game->SetStorage (*storage);

      game->SetGameLogic (rules);

      if (config.PendingMoves != nullptr)
        game->SetPendingMoveProcessor (*config.PendingMoves);

      if (config.EnablePruning >= 0)
        game->EnablePruning (config.EnablePruning);

      auto components = instanceFact->BuildGameComponents (*game);

      auto serverConnector = CreateRpcServerConnector (config);
      if (serverConnector == nullptr)
          LOG (WARNING)
              << "No connector has been set up for the game RPC server,"
                 " no RPC interface will be available";
      else
          components.push_back (
              instanceFact->BuildRpcServer (*game, *serverConnector));

      for (auto& c : components)
        c->Start ();
      game->Run ();
      for (auto& c : components)
        c->Stop ();

      /* We need to make sure that the Game instance is destructed before the
         storage is.  That is necessary, since destructing the Game instance
         may still cause some batched transactions to be flushed, and this
         needs the storage intact.  */
      game.reset ();
    }
  catch (const std::exception& exc)
    {
      LOG (FATAL) << "Exception caught: " << exc.what ();
    }
  catch (...)
    {
      LOG (FATAL) << "Unknown exception caught";
    }

  return EXIT_SUCCESS;
}

int
SQLiteMain (const GameDaemonConfiguration& config, const std::string& gameId,
            SQLiteGame& rules)
{
  try
    {
      CustomisedInstanceFactory* instanceFact = config.InstanceFactory;
      std::unique_ptr<CustomisedInstanceFactory> defaultInstanceFact;
      if (instanceFact == nullptr)
        {
          defaultInstanceFact = std::make_unique<CustomisedInstanceFactory> ();
          instanceFact = defaultInstanceFact.get ();
        }

      CHECK (!config.SpaceXpanseRpcUrl.empty ()) << "SpaceXpanseRpcUrl must be configured";
      const std::string jsonRpcUrl(config.SpaceXpanseRpcUrl);
      jsonrpc::HttpClient httpConnector(jsonRpcUrl);

      const auto protocol = GetClientVersion (config);
      if (config.SpaceXpanseRpcWait)
        WaitForSpaceXpanse (httpConnector, protocol);

      auto game = std::make_unique<Game> (gameId);
      game->ConnectRpcClient (httpConnector, protocol);
      VerifySpaceXpanseVersion (config, game->GetSpaceXpanseVersion ());
      CHECK (game->DetectZmqEndpoint ());

      const fs::path gameDir = GetGameDirectory (config, gameId,
                                                 game->GetChain ());
      const fs::path dbFile = gameDir / fs::path ("storage.sqlite");

      rules.Initialise (dbFile.string ());
      game->SetStorage (rules.GetStorage ());

      game->SetGameLogic (rules);

      if (config.PendingMoves != nullptr)
        game->SetPendingMoveProcessor (*config.PendingMoves);

      if (config.EnablePruning >= 0)
        game->EnablePruning (config.EnablePruning);

      auto components = instanceFact->BuildGameComponents (*game);

      auto serverConnector = CreateRpcServerConnector (config);
      if (serverConnector == nullptr)
          LOG (WARNING)
              << "No connector has been set up for the game RPC server,"
                 " no RPC interface will be available";
      else
          components.push_back (
              instanceFact->BuildRpcServer (*game, *serverConnector));

      for (auto& c : components)
        c->Start ();
      game->Run ();
      for (auto& c : components)
        c->Stop ();

      /* We need to make sure that the Game instance is destructed before the
         storage is.  That is necessary, since destructing the Game instance
         may still cause some batched transactions to be flushed, and this
         needs the storage intact.  */
      game.reset ();
    }
  catch (const std::exception& exc)
    {
      LOG (FATAL) << "Exception caught: " << exc.what ();
    }
  catch (...)
    {
      LOG (FATAL) << "Unknown exception caught";
    }

  return EXIT_SUCCESS;
}

namespace
{

class CallbackGameLogic : public GameLogic
{

private:

  /** The callback pointers.  */
  const GameLogicCallbacks& callbacks;

protected:

  GameStateData
  GetInitialStateInternal (unsigned& height, std::string& hashHex) override
  {
    CHECK (callbacks.GetInitialState != nullptr);
    return callbacks.GetInitialState (GetContext ().GetChain (),
                                      height, hashHex);
  }

  GameStateData
  ProcessForwardInternal (const GameStateData& oldState,
                          const Json::Value& blockData,
                          UndoData& undoData) override
  {
    CHECK (callbacks.ProcessForward != nullptr);
    return callbacks.ProcessForward (GetContext ().GetChain (),
                                     oldState, blockData, undoData);
  }

  GameStateData
  ProcessBackwardsInternal (const GameStateData& oldState,
                            const Json::Value& blockData,
                            const UndoData& undoData) override
  {
    CHECK (callbacks.ProcessBackwards != nullptr);
    return callbacks.ProcessBackwards (GetContext ().GetChain (),
                                       oldState, blockData, undoData);
  }

public:

  explicit CallbackGameLogic (const GameLogicCallbacks& cb)
    : callbacks(cb)
  {}

  Json::Value
  GameStateToJson (const GameStateData& state) override
  {
    if (callbacks.GameStateToJson != nullptr)
      return callbacks.GameStateToJson (state);
    return GameLogic::GameStateToJson (state);
  }

};

} // anonymous namespace

int
DefaultMain (const GameDaemonConfiguration& config, const std::string& gameId,
             const GameLogicCallbacks& callbacks)
{
  CallbackGameLogic rules(callbacks);
  return DefaultMain (config, gameId, rules);
}

} // namespace spacexpanse
