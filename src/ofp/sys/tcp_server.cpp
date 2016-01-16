// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/tcp_server.h"
#include "ofp/sys/engine.h"
#include "ofp/log.h"
#include "ofp/sys/tcp_connection.h"
#include "ofp/sys/udp_server.h"

using namespace ofp::sys;

std::shared_ptr<TCP_Server> TCP_Server::create(
    Engine *engine, ChannelMode mode, UInt64 securityId,
    const IPv6Endpoint &localEndpt, ProtocolVersions versions,
    ChannelListener::Factory listenerFactory, std::error_code &error) {
  auto ptr =
      std::make_shared<TCP_Server>(PrivateToken{}, engine, mode, securityId,
                                   localEndpt, versions, listenerFactory);
  ptr->asyncListen(localEndpt, error);
  return ptr;
}

TCP_Server::TCP_Server(PrivateToken t, Engine *engine, ChannelMode mode,
                       UInt64 securityId, const IPv6Endpoint &localEndpt,
                       ProtocolVersions versions,
                       ChannelListener::Factory listenerFactory)
    : engine_{engine},
      acceptor_{engine->io()},
      socket_{engine->io()},
      mode_{mode},
      versions_{versions},
      factory_{listenerFactory},
      securityId_{securityId} {
}

TCP_Server::~TCP_Server() {
  // If connId_ is non-zero, we need to de-register the TCP server.
  //
  // If connId_ is zero, there was an error opening when constructing the TCP
  // server, and there is nothing left to destroy.

  if (connId_) {
    // Dispose of UDP server first.
    udpServer_->shutdown();

    log::info("Stop listening on TCP", std::make_pair("tlsid", securityId_),
              std::make_pair("connid", connId_));
    engine_->releaseServer(this);
  }
}

ofp::IPv6Endpoint TCP_Server::localEndpoint() const {
  asio::error_code err;
  return convertEndpoint<tcp>(acceptor_.local_endpoint(err));
}

void TCP_Server::shutdown() {
  acceptor_.close();
  udpServer_->shutdown();
}

void TCP_Server::asyncListen(const IPv6Endpoint &localEndpt,
                             std::error_code &error) {
  listen(localEndpt, error);

  if (!error) {
    connId_ = engine_->registerServer(this);
    log::info("Start listening on TCP", localEndpt,
              std::make_pair("tlsid", securityId_),
              std::make_pair("connid", connId_));
    asyncAccept();
    listenUDP(localEndpt, error);

  } else {
    connId_ = 0;
    log::error("Listen failed on TCP", localEndpt, error);
  }
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
  auto self(this->shared_from_this());

  acceptor_.async_accept(socket_, [this, self](const asio::error_code &err) {
    // N.B. ASIO still sends a cancellation error even after
    // async_accept() throws an exception. Check for cancelled operation
    // first; our TCP_Server instance will have been destroyed.
    if (err == asio::error::operation_aborted)
      return;

    if (!err) {
      if (securityId_ > 0) {
        auto conn = std::make_shared<TCP_Connection<EncryptedSocket>>(
            engine_, std::move(socket_), mode_, securityId_, versions_,
            factory_);
        conn->asyncAccept();
      } else {
        auto conn = std::make_shared<TCP_Connection<PlaintextSocket>>(
            engine_, std::move(socket_), mode_, securityId_, versions_,
            factory_);
        conn->asyncAccept();
      }

    } else {
      log::error("Error in TCP_Server.asyncAcept:", err);
    }

    asyncAccept();
  });
}

void TCP_Server::listenUDP(const IPv6Endpoint &localEndpt,
                           std::error_code &error) {
  udpServer_ = UDP_Server::create(engine_, mode_, securityId_, localEndpt,
                                  versions_, connId_, error);
}
