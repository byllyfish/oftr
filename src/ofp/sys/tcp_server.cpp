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

using namespace ofp::sys;

TCP_Server::TCP_Server(Engine *engine, Driver::Role role,
                       const tcp::endpoint &endpt, ProtocolVersions versions,
                       ChannelListener::Factory listenerFactory,
                       std::error_code &error)
    : engine_{engine}, acceptor_{engine->io()}, socket_{engine->io()},
      role_{role}, versions_{versions}, factory_{listenerFactory} {

  listen(endpt, error);

  if (!error)
    asyncAccept();

  engine_->registerServer(this);

  log::info("Start TCP listening on", endpt);
}

TCP_Server::~TCP_Server() {
  asio::error_code err;
  tcp::endpoint endpt = acceptor_.local_endpoint(err);

  log::info("Stop TCP listening on", endpt);
  engine_->releaseServer(this);
}

void TCP_Server::listen(const tcp::endpoint &localEndpt,
                        std::error_code &error) {
  auto endpt = localEndpt;
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
      auto conn = std::make_shared<TCP_Connection>(engine_, std::move(socket_),
                                                   role_, versions_, factory_);
      conn->asyncAccept();

    } else {
      log::error("Error in TCP_Server.asyncAcept:", err);
    }

    asyncAccept();
  });
}
