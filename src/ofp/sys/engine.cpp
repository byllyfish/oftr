//  ===== ---- ofp/sys/engine.cpp --------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements sys::Engine class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/sys/engine.h"
#include "ofp/log.h"
#include "ofp/sys/tcp_server.h"
#include "ofp/sys/udp_server.h"
#include "ofp/defaultauxiliarylistener.h"
#include "ofp/sys/tcp_connection.h"

using namespace ofp::sys;
using ofp::UInt64;

Engine::Engine(Driver *driver)
    : driver_{driver}, context_{asio::ssl::context::tlsv1}, signals_{io_},
      stopTimer_{io_} {
  log::debug("Engine ready");
}

Engine::~Engine() {
  // Connections and servers will remove themselves when they are destroyed.
  // When the engine is destroyed, this bookkeeping doesn't matter anymore. To
  // make destruction "faster", we can clear the data structures first.

  dpidMap_.clear();
  serverList_.clear();
  connList_.clear();

  log::debug("Engine shutting down");
}

std::error_code
Engine::configureTLS(const std::string &privateKeyFile,
                     const std::string &certificateFile,
                     const std::string &certificateAuthorityFile,
                     const char *privateKeyPassword) {
  asio::error_code error;

  // Even if there's an error, consider TLS desired.
  isTLSDesired_ = true;

  context_.set_options(
      asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3, error);
  if (error)
    return error;

  if (privateKeyPassword && privateKeyPassword[0] > 0) {
    context_.set_password_callback(
        [privateKeyPassword](std::size_t max_length,
                             asio::ssl::context::password_purpose purpose) {
          return privateKeyPassword;
        },
        error);
    if (error)
      return error;
  }

  context_.use_certificate_chain_file(certificateFile, error);
  if (error)
    return error;

  context_.use_private_key_file(privateKeyFile, asio::ssl::context::pem, error);
  if (error)
    return error;

  // TODO certificateAuthorityFile.

  return error;
}

UInt64 Engine::listen(ChannelMode mode,
                               const IPv6Endpoint &localEndpoint,
                               ProtocolVersions versions,
                               ChannelListener::Factory listenerFactory, std::error_code &error) {
  auto svrPtr = TCP_Server::create(this, mode, localEndpoint, versions,
                                          listenerFactory, error);
  if (error)
    return 0;

  // If there's no error, the TCP_Server has registered itself with the engine.
  UInt64 connId = svrPtr->connectionId();
  assert(connId > 0);
  
  // Register signal handlers.
  installSignalHandlers();

  return connId;
}

UInt64 Engine::connect(ChannelMode mode, const IPv6Endpoint &remoteEndpoint,
                        ProtocolVersions versions, ChannelListener::Factory listenerFactory,
                        std::function<void(Channel*,std::error_code)> resultHandler) {
  UInt64 connId;

  if (isTLSDesired()) {
    auto connPtr = std::make_shared<TCP_Connection<EncryptedSocket>>(this, mode, versions, listenerFactory);
    connPtr->asyncConnect(remoteEndpoint, resultHandler);
    connId = connPtr->connectionId();
  } else {
    auto connPtr = std::make_shared<TCP_Connection<PlaintextSocket>>(this, mode, versions, listenerFactory);
    connPtr->asyncConnect(remoteEndpoint, resultHandler);
    connId = connPtr->connectionId();
  }

  return connId;
}

void Engine::run() {
  if (!isRunning_) {
    isRunning_ = true;

    // Set isRunning_ to true when we are in io.run(). This guards against
    // re-entry and provides a flag to test when shutting down.

    io_.run();

    isRunning_ = false;
  }
}

void Engine::stop(Milliseconds timeout) {
  if (timeout == 0_ms) {
    io_.stop();
  } else {
    asio::error_code error;
    stopTimer_.expires_from_now(timeout, error);
    if (error) {
      io_.stop();
      return;
    }

    stopTimer_.async_wait([this](const asio::error_code &err) {
      if (err != asio::error::operation_aborted) {
        io_.stop();
      }
    });
  }
}

#if 0
void Engine::openAuxChannel(UInt8 auxID, Channel::Transport transport,
                            Connection *mainConnection) {
  // Find the localport of the mainConnection.
  // Look up the apppropriate server object if necessary (for UDP).
  // Create connection using AuxChannelListener and set up connection to main
  // connection. Note that deferred from connection attempt must be sent to
  // mainConnection.

  if (transport == Channel::Transport::TCP) {
    log::debug("openAuxChannel", auxID);

    tcp::endpoint endpt =
        convertEndpoint<tcp>(mainConnection->remoteEndpoint());
    DefaultHandshake *hs = mainConnection->handshake();
    ProtocolVersions versions = hs->versions();

    ChannelListener::Factory listenerFactory = []() {
      return new DefaultAuxiliaryListener;
    };

    // FIXME should Auxiliary connections use a null listenerFactory? (Use
    // defaultauxiliarylistener by default?)
    auto connPtr = std::make_shared<TCP_Connection<PlaintextSocket>>(
        this, ChannelMode::Auxiliary, versions, listenerFactory);

    // FIXME we used to set datapathID and auxiliaryId here...
    connPtr->setMainConnection(mainConnection, auxID);

    Deferred<std::error_code> result = connPtr->asyncConnect(endpt);

    // FIXME where does the exception go?
    // result.done([mainConnection](Exception exc){
    //  mainConnection
    //});
  }
}
#endif //0

void Engine::registerDatapath(Connection *channel) {
  DatapathID dpid = channel->datapathId();
  UInt8 auxID = channel->auxiliaryId();

  if (auxID == 0) {
    // Insert main connection's datapathID into the dpidMap, if not present
    // already.
    auto pair = dpidMap_.insert({dpid, channel});
    if (!pair.second) {
      // DatapathID already exists in map. If the existing channel is
      // different, close it and replace it with the new one.
      auto item = pair.first;
      if (item->second != channel) {
        log::info("Engine.postDatapathID: Conflict between main connections "
                  "detected. Closing old.",
                  dpid);
        Connection *old = item->second;
        item->second = channel;
        old->shutdown();

      } else {
        log::info("Engine.postDatapathID: DatapathID is already registered.",
                  dpid);
      }
    }

  } else {
    assert(auxID != 0);

    // Look up main connection and assign auxiliary connection to it. If we
    // don't find a main connection, close the auxiliary channel.
    auto item = dpidMap_.find(dpid);
    if (item != dpidMap_.end()) {
      channel->setMainConnection(item->second, auxID);

    } else {
      log::info("Engine.postDatapathID: Main connection not found.", dpid);
      channel->shutdown();
    }
  }
}

void Engine::releaseDatapath(Connection *channel) {
  DatapathID dpid = channel->datapathId();
  UInt8 auxID = channel->auxiliaryId();

  if (auxID == 0) {
    // When releasing the datapathID for a main connection, we need to
    // verify that the datapathID _is_ registered to this connection.
    auto item = dpidMap_.find(dpid);
    if (item != dpidMap_.end()) {
      if (item->second == channel)
        dpidMap_.erase(item);
    } else if (isRunning_) {
      log::debug("Engine.releaseDatapathID: Unable to find datapathID.", dpid);
    }
  } else {
    log::debug("Engine.releaseDatapathID called for auxID", auxID);
  }
}

UInt64 Engine::registerServer(TCP_Server *server) { 
  serverList_.push_back(server);
  return assignConnId();
}

void Engine::releaseServer(TCP_Server *server) {
  auto iter = std::find(serverList_.begin(), serverList_.end(), server);
  if (iter != serverList_.end()) {
    serverList_.erase(iter);
  }
}

UInt64 Engine::registerConnection(Connection *connection) {
  connList_.push_back(connection);
  return assignConnId();
}

void Engine::releaseConnection(Connection *connection) {
  auto iter = std::find(connList_.begin(), connList_.end(), connection);
  if (iter != connList_.end()) {
    std::swap(*iter, connList_.back());
    connList_.pop_back();
  }
}


void Engine::installSignalHandlers() {
  if (!isSignalsInited_) {
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    signals_.async_wait([this](const asio::error_code &error, int signum) {
      if (!error) {
        log::info("Signal received:", signum);
        this->stop();
      }
    });
  }
}

UInt64 Engine::assignConnId() {
  UInt64 id = ++lastConnId_;
  if (id == 0) {
    id = ++lastConnId_;
  }
  return id;
}


Connection *Engine::findDatapath(const DatapathID &dpid) const {
  auto item = dpidMap_.find(dpid);
  if (item != dpidMap_.end()) {
    return item->second;
  }
  return nullptr;
}
