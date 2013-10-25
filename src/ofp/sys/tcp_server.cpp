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

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

TCP_Server::TCP_Server(Engine *engine, Driver::Role role,
                       const Features *features, const tcp::endpoint &endpt,
                       ProtocolVersions versions,
                       ChannelListener::Factory listenerFactory)
    : engine_{engine}, acceptor_{engine->io()}, socket_{engine->io()},
      role_{role}, versions_{versions}, factory_{listenerFactory} {
  listen(endpt);

  if (features) {
    features_ = *features;
  }

  asyncAccept();

  engine_->registerServer(this);

  log::info("Start TCP listening on", endpt);
}

TCP_Server::~TCP_Server() {
  error_code err;
  tcp::endpoint endpt = acceptor_.local_endpoint(err);

  log::info("Stop TCP listening on", endpt);
  engine_->releaseServer(this);
}

void TCP_Server::listen(const tcp::endpoint &endpt) {
  // Handle case where IPv6 is not supported on this system.
  tcp::endpoint ep = endpt;
  try {
    acceptor_.open(ep.protocol());
  }
  catch (boost::system::system_error &ex) {
    auto addr = ep.address();
    if (ex.code() == boost::asio::error::address_family_not_supported &&
        addr.is_v6() && addr.is_unspecified()) {
      log::info("TCP_Server: IPv6 is not supported. Using IPv4.");
      ep = tcp::endpoint{tcp::v4(), ep.port()};
      acceptor_.open(ep.protocol());
    } else {
      log::debug("TCP_Server::listen - unexpected exception", ex.code());
      throw;
    }
  }

  acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
  acceptor_.bind(ep);
  acceptor_.listen(boost::asio::socket_base::max_connections);
}

void TCP_Server::asyncAccept() {
  acceptor_.async_accept(socket_, [this](error_code err) {
    // N.B. ASIO still sends a cancellation error even after
    // async_accept() throws an exception. Check for cancelled operation
    // first; our TCP_Server instance will have been destroyed.
    if (isAsioCanceled(err))
      return;

    log::Lifetime lifetime("async_accept callback");
    if (!err) {
      auto conn = std::make_shared<TCP_Connection>(engine_, std::move(socket_),
                                                   role_, versions_, factory_);
      conn->setFeatures(features_);
      conn->asyncAccept();

    } else {
      Exception exc = makeException(err);
      log::error("Error in TCP_Server.asyncAcept:", exc.toString());
    }

    asyncAccept();
  });
}

} // </namespace sys>
} // </namespace ofp>
