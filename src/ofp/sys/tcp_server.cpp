// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/tcp_server.h"

#include "ofp/log.h"
#include "ofp/sys/engine.h"
#include "ofp/sys/tcp_connection.h"

using namespace ofp;
using namespace ofp::sys;

std::shared_ptr<TCP_Server> TCP_Server::create(
    Engine *engine, ChannelOptions options, UInt64 securityId,
    const IPv6Endpoint &localEndpt, ProtocolVersions versions,
    ChannelListener::Factory listenerFactory, std::error_code &error) {
  auto ptr =
      std::make_shared<TCP_Server>(PrivateToken{}, engine, options, securityId,
                                   localEndpt, versions, listenerFactory);
  ptr->asyncListen(localEndpt, error);
  return ptr;
}

TCP_Server::TCP_Server(PrivateToken t, Engine *engine, ChannelOptions options,
                       UInt64 securityId, const IPv6Endpoint &localEndpt,
                       ProtocolVersions versions,
                       ChannelListener::Factory listenerFactory)
    : engine_{engine},
      acceptor_{engine->io()},
      options_{options},
      versions_{versions},
      factory_{listenerFactory},
      securityId_{securityId} {}

TCP_Server::~TCP_Server() {
  // If connId_ is non-zero, we need to de-register the TCP server.
  //
  // If connId_ is zero, there was an error opening when constructing the TCP
  // server, and there is nothing left to destroy.

  if (connId_) {
    log_info("Stop listening on TCP", std::make_pair("tlsid", securityId_),
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
}

void TCP_Server::asyncListen(const IPv6Endpoint &localEndpt,
                             std::error_code &error) {
  listen(localEndpt, error);

  if (!error) {
    connId_ = engine_->registerServer(this);
    log_info("Start listening on TCP", localEndpt,
             std::make_pair("tlsid", securityId_),
             std::make_pair("connid", connId_));
    asyncAccept();

  } else {
    connId_ = 0;
    log_error("Listen failed on TCP", localEndpt, error);
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
    log_info("TCP_Server: IPv6 is not supported. Using IPv4.");
    endpt = tcp::endpoint{tcp::v4(), endpt.port()};
    acceptor_.open(endpt.protocol(), error);
    if (error)
      return;
  }

  acceptor_.set_option(asio::socket_base::reuse_address(true), error);
  if (error)
    return;

  acceptor_.bind(endpt, error);
  if (error)
    return;

  acceptor_.listen(asio::socket_base::max_listen_connections, error);
}

void TCP_Server::asyncAccept() {
  auto self(this->shared_from_this());

  acceptor_.async_accept([this, self](const asio::error_code &err,
                                      tcp::socket socket) {
    // N.B. ASIO still sends a cancellation error even after
    // async_accept() throws an exception. Check for cancelled operation
    // first; our TCP_Server instance will have been destroyed.
    if (err == asio::error::operation_aborted)
      return;

    if (!err) {
      if (securityId_ > 0) {
        TCP_AsyncAccept<EncryptedSocket>(engine_, std::move(socket), options_,
                                         securityId_, versions_, factory_);
      } else {
        TCP_AsyncAccept<PlaintextSocket>(engine_, std::move(socket), options_,
                                         securityId_, versions_, factory_);
      }

    } else {
      log_error("Error in TCP_Server.asyncAccept:", err);
    }

    asyncAccept();
  });
}
