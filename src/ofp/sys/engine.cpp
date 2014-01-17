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

Engine::Engine(Driver *driver)
    : driver_{driver}, context_{asio::ssl::context::tlsv1}, signals_{io_},
      stopTimer_{io_} {}

Engine::~Engine() {
  // Copy the serverlist into a temporary and clear the original list to help
  // speed things up; servers attempt to remove themselves from the server
  // list when they are destroyed.

  ServerList servers;
  std::swap(servers, serverList_);
  assert(serverList_.empty());

  for (auto svr : servers) {
    delete svr;
  }
}

std::error_code
Engine::configureTLS(const std::string &privateKeyFile,
                     const std::string &certificateFile,
                     const std::string &certificateAuthorityFile,
                     const char *privateKeyPassword) {
  asio::error_code error;

  // Even if there's an error, consider TLS desired.
  isTLSDesired = true;

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

ofp::Deferred<std::error_code>
Engine::listen(Driver::Role role, const IPv6Endpoint &localEndpoint,
               ProtocolVersions versions,
               ChannelListener::Factory listenerFactory) {
  auto tcpEndpt = convertEndpoint<tcp>(localEndpoint);
  auto udpEndpt = convertEndpoint<udp>(localEndpoint);
  auto result = Deferred<std::error_code>::makeResult();

  std::error_code error;

  auto tcpsvr = MakeUniquePtr<TCP_Server>(this, role, tcpEndpt, versions,
                                          listenerFactory, error);

  if (error) {
    result->done(error);
    return result;
  }

  auto udpsvr =
      MakeUniquePtr<UDP_Server>(this, role, udpEndpt, versions, error);

  if (error) {
    result->done(error);
    return result;
  }

  (void)tcpsvr.release();
  (void)udpsvr.release();

  // Register signal handlers.
  installSignalHandlers();

  result->done(std::error_code{});

  return result;
}

ofp::Deferred<std::error_code>
Engine::connect(Driver::Role role, const IPv6Endpoint &remoteEndpoint,
                ProtocolVersions versions,
                ChannelListener::Factory listenerFactory) {
  tcp::endpoint endpt = convertEndpoint<tcp>(remoteEndpoint);

  auto connPtr =
      std::make_shared<TCP_Connection<PlaintextSocket>>(this, role, versions, listenerFactory);

  // If the role is `Agent`, the connection will keep retrying. Install signal
  // handlers to tell it to stop.
  if (role == Driver::Agent) {
    installSignalHandlers();
  }

  auto result = connPtr->asyncConnect(endpt);

  if (role == Driver::Agent) {
    // When the role is Agent, the connection will keep trying to reconnect.
    // In this case, we always return no error.
    return std::error_code{};
  }

  return result;
}

void Engine::reconnect(DefaultHandshake *handshake,
                       const IPv6Endpoint &remoteEndpoint,
                       std::chrono::milliseconds delay) {
  tcp::endpoint endpt = convertEndpoint<tcp>(remoteEndpoint);

  auto connPtr = std::make_shared<TCP_Connection<PlaintextSocket>>(this, handshake);
  (void)connPtr->asyncConnect(endpt, delay);
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
    auto connPtr = std::make_shared<TCP_Connection<PlaintextSocket>>(this, Driver::Auxiliary,
                                                    versions, listenerFactory);

    // FIXME we used to set datapathID and auxiliaryId here...
    connPtr->setMainConnection(mainConnection, auxID);

    Deferred<std::error_code> result = connPtr->asyncConnect(endpt);

    // FIXME where does the exception go?
    // result.done([mainConnection](Exception exc){
    //  mainConnection
    //});
  }
}

void Engine::postDatapathID(Connection *channel) {
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

void Engine::releaseDatapathID(Connection *channel) {
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

void Engine::registerServer(Server *server) { serverList_.push_back(server); }

void Engine::releaseServer(Server *server) {
  auto iter = std::find(serverList_.begin(), serverList_.end(), server);
  if (iter != serverList_.end()) {
    serverList_.erase(iter);
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
