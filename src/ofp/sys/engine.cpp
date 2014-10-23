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
#include "ofp/sys/tcp_connection.h"

using namespace ofp::sys;
using ofp::UInt64;

Engine::Engine(Driver *driver)
    : driver_{driver}, signals_{io_},
      stopTimer_{io_} {
  log::info("Engine ready");
}

Engine::~Engine() {
  // Shutdown all existing connections and servers.
  (void)close(0);
  log::info("Engine shutting down");
}


UInt64 Engine::listen(ChannelMode mode, UInt64 securityId, 
                               const IPv6Endpoint &localEndpoint,
                               ProtocolVersions versions,
                               ChannelListener::Factory listenerFactory, std::error_code &error) {
  auto svrPtr = TCP_Server::create(this, mode, securityId, localEndpoint, versions,
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

UInt64 Engine::connect(ChannelMode mode, UInt64 securityId, const IPv6Endpoint &remoteEndpoint,
                        ProtocolVersions versions, ChannelListener::Factory listenerFactory,
                        std::function<void(Channel*,std::error_code)> resultHandler) {
  UInt64 connId = 0;

  if (securityId != 0) {
    auto connPtr = std::make_shared<TCP_Connection<EncryptedSocket>>(this, mode, securityId, versions, listenerFactory);
    connPtr->asyncConnect(remoteEndpoint, resultHandler);
    connId = connPtr->connectionId();

  } else {
    auto connPtr = std::make_shared<TCP_Connection<PlaintextSocket>>(this, mode, securityId, versions, listenerFactory);
    connPtr->asyncConnect(remoteEndpoint, resultHandler);
    connId = connPtr->connectionId();
  }

  return connId;
}

UInt64 Engine::connectUDP(ChannelMode mode, UInt64 securityId, const IPv6Endpoint &remoteEndpoint,
                        ProtocolVersions versions, ChannelListener::Factory listenerFactory,
                        std::error_code &error) {
  UInt64 connId = 0;

  if (!udpConnect_) {
    udpConnect_ = UDP_Server::create(this, error);
    if (error) {
      udpConnect_.reset();
    }
  }

  if (udpConnect_) {
    connId = udpConnect_->connect(remoteEndpoint, listenerFactory, error);
  }

  return connId;
}

size_t Engine::close(UInt64 connId) {
  if (connId != 0) {
    // Close a specific server or connection.
    TCP_Server *server = findTCPServer([connId](TCP_Server *svr) {
      return svr->connectionId() == connId;
    });

    if (server) {
      server->shutdown();
      return 1;
    }

    Channel *channel = findConnection([connId](Channel *chan) {
      return chan->connectionId() == connId;
    });

    if (channel) {
      channel->shutdown();
      return 1;
    }  

    return 0;

  } else {
    // Close all servers and connections.
    size_t result = serverList_.size() + connList_.size();

    ServerList servers;
    servers.swap(serverList_);
    for (auto svr: servers) {
      svr->shutdown();
    }

    ConnectionList conns;
    conns.swap(connList_);
    for (auto chan: conns) {
      chan->shutdown();
    }

    if (udpConnect_) {
      udpConnect_->shutdown();
      udpConnect_.reset();
      ++result;
    }

    return result;
  }
}

UInt64 Engine::addIdentity(const std::string &certFile, const std::string &password, const std::string &verifier, std::error_code &error) {
  auto idPtr = MakeUniquePtr<Identity>(certFile, password, verifier, error);
  if (error)
    return 0;

  UInt64 secId = assignSecurityId();
  assert(secId > 0);

  idPtr->setSecurityId(secId);

  log::info("Add TLS identity", std::make_pair("tlsid", secId));

  identities_.push_back(std::move(idPtr));

  return secId;
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

asio::ssl::context *Engine::securityContext(UInt64 securityId) {
  if (securityId == 0) {
    return nullptr;
  }

  auto iter = std::find_if(identities_.begin(), identities_.end(), 
    [securityId](std::unique_ptr<Identity> &identity) {
      return identity->securityId() == securityId;
    });

  if (iter != identities_.end()) {
    return iter->get()->securityContext();
  }

  return nullptr;
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

bool Engine::registerDatapath(Connection *channel) {
  DatapathID dpid = channel->datapathId();
  UInt8 auxID = channel->auxiliaryId();

  if (auxID == 0) {
    assert(!IsChannelTransportUDP(channel->transport()));

    // Insert main connection's datapathID into the dpidMap, if not present
    // already.
    auto pair = dpidMap_.insert({dpid, channel});
    if (!pair.second) {
      // DatapathID already exists in map. If the existing channel is
      // different, close it and replace it with the new one.
      auto item = pair.first;
      if (item->second != channel) {
        log::error("registerDatapath: Conflict between main connections detected",
                  dpid, std::make_pair("conn_id", channel->connectionId()));
        Connection *old = item->second;
        item->second = channel;
        old->shutdown();

      } else {
        log::error("registerDatapath: Datapath is already registered.",
                  dpid, std::make_pair("conn_id", channel->connectionId()));
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
      log::error("registerDatapath: Main connection not found", dpid, std::make_pair("conn_id", channel->connectionId()));
      return false;
    }
  }

  return true;
}

void Engine::releaseDatapath(Connection *channel) {
  DatapathID dpid = channel->datapathId();
  UInt8 auxID = channel->auxiliaryId();

  // We only need to release the datapath for main connections.
  if (auxID == 0) {
    // When releasing the datapathID for a main connection, we need to
    // verify that the datapathID _is_ registered to this connection.
    auto item = dpidMap_.find(dpid);
    if (item != dpidMap_.end()) {
      if (item->second == channel)
        dpidMap_.erase(item);
    }
  }
}

UInt64 Engine::registerServer(TCP_Server *server) { 
  assert(!serverListLock_);
  serverList_.push_back(server);
  return assignConnectionId();
}

void Engine::releaseServer(TCP_Server *server) {
  assert(!serverListLock_);
  auto iter = std::find(serverList_.begin(), serverList_.end(), server);
  if (iter != serverList_.end()) {
    assert(*iter == server);
    serverList_.erase(iter);
  }
}

UInt64 Engine::registerConnection(Connection *connection) {
  assert(!connListLock_);
  connList_.push_back(connection);
  return assignConnectionId();
}

void Engine::releaseConnection(Connection *connection) {
  assert(!connListLock_);
  auto iter = std::find(connList_.begin(), connList_.end(), connection);
  if (iter != connList_.end()) {
    assert(*iter == connection);
    connList_.erase(iter);
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

UInt64 Engine::assignConnectionId() {
  // FIXME: handle case where connectionId wraps around...
  if (++lastConnId_ == 0) {
    lastConnId_ = 1;
  }
  return lastConnId_;
}

UInt64 Engine::assignSecurityId() {
  if (++lastSecurityId_ == 0) {
    lastSecurityId_ = 1;
  }
  return lastSecurityId_;
}


Connection *Engine::findDatapath(const DatapathID &dpid, UInt64 connId) const {
  // If datapath ID is not all zeros, use it to look up the connection.
  if (!dpid.empty()) {
    auto item = dpidMap_.find(dpid);
    if (item != dpidMap_.end()) {
      return item->second;
    }

  } else if (connId != 0) {
    // Otherwise use the connectionId, it it's non-zero.
    return findConnection([connId](Channel *channel) {
      return channel->connectionId() == connId;
    });
  }


  return nullptr;
}
