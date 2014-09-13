//  ===== ---- ofp/sys/tcp_server.cpp ----------------------*- C++ -*- =====  //
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
/// \brief Implements TCP_Server class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/sys/tcp_server.h"
#include "ofp/sys/engine.h"
#include "ofp/log.h"
#include "ofp/sys/tcp_connection.h"
#include "ofp/sys/udp_server.h"

using namespace ofp::sys;

TCP_Server::TCP_Server(Engine *engine, ChannelMode mode,
                       const IPv6Endpoint &localEndpt, ProtocolVersions versions,
                       ChannelListener::Factory listenerFactory,
                       std::error_code &error)
    : engine_{engine}, acceptor_{engine->io()}, socket_{engine->io()},
      mode_{mode}, versions_{versions}, factory_{listenerFactory} {

  listen(localEndpt, error);

  if (!error) {
    connId_ = engine_->registerServer(this);
    log::info("Start listening on TCP", localEndpt, std::make_pair("connid", connId_));
    asyncAccept();
    listenUDP(localEndpt, error);

  } else {
    connId_ = 0;
    log::error("Listen failed on TCP", localEndpt, error);
  }
}

TCP_Server::~TCP_Server() {
  if (connId_) {
    // Dispose of UDP server first.
    udpServer_.reset();

    log::info("Stop listening on TCP", std::make_pair("connid", connId_));
    engine_->releaseServer(this);
  }
}


ofp::IPv6Endpoint TCP_Server::localEndpoint() const {
  asio::error_code err;
  tcp::endpoint endpt = acceptor_.local_endpoint(err);
  return convertEndpoint<tcp>(endpt);
}


void TCP_Server::listen(const IPv6Endpoint &localEndpt,
                        std::error_code &error) {
  auto endpt = convertEndpoint<tcp>(localEndpt);
  auto addr = endpt.address();

  acceptor_.open(endpt.protocol(), error);

  // Handle case where IPv6 is not supported on this system.
  if (error == asio::error::address_family_not_supported && addr.is_v6() &&
      addr.is_unspecified()) {
    log::info("TCP_Server: IPv6 is not supported. Using IPv4.");
    endpt = tcp::endpoint{tcp::v4(), endpt.port()};
    if (acceptor_.open(endpt.protocol(), error))
      return;
  }

  if (acceptor_.set_option(asio::socket_base::reuse_address(true), error))
    return;

  if (acceptor_.bind(endpt, error))
    return;

  acceptor_.listen(asio::socket_base::max_connections, error);
}

void TCP_Server::asyncAccept() {
  acceptor_.async_accept(socket_, [this](const asio::error_code &err) {
    // N.B. ASIO still sends a cancellation error even after
    // async_accept() throws an exception. Check for cancelled operation
    // first; our TCP_Server instance will have been destroyed.
    if (err == asio::error::operation_aborted)
      return;

    log::Lifetime lifetime("async_accept callback");
    if (!err) {

      if (engine_->isTLSDesired()) {
        auto conn = std::make_shared<TCP_Connection<EncryptedSocket>>(
            engine_, std::move(socket_), mode_, versions_, factory_);
        conn->asyncAccept();
      } else {
        auto conn = std::make_shared<TCP_Connection<PlaintextSocket>>(
            engine_, std::move(socket_), mode_, versions_, factory_);
        conn->asyncAccept();
      }

    } else {
      log::error("Error in TCP_Server.asyncAcept:", err);
    }

    asyncAccept();
  });
}


void TCP_Server::listenUDP(const IPv6Endpoint &localEndpt, std::error_code &error) {
  udpServer_ = MakeUniquePtr<UDP_Server>(engine_, mode_, localEndpt, versions_, connId_, error);
}
