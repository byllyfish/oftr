// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/sys/udp_server.h"
#include "ofp/sys/udp_connection.h"
#include "ofp/sys/engine.h"
#include "ofp/echorequest.h"
#include "ofp/echoreply.h"
#include "ofp/hello.h"

using namespace ofp;
using namespace ofp::sys;

std::shared_ptr<UDP_Server> UDP_Server::create(Engine *engine, ChannelMode mode,
                                               UInt64 securityId,
                                               const IPv6Endpoint &localEndpt,
                                               ProtocolVersions versions,
                                               UInt64 connId,
                                               std::error_code &error) {
  auto ptr = std::make_shared<UDP_Server>(PrivateToken{}, engine, mode, securityId,
                                          versions, connId);
  ptr->asyncListen(localEndpt, error);
  return ptr;
}

/// \brief Construct a UDP_Server for use in making outgoing connections.
///
/// The server will open a UDP socket in the ephemeral port range.
std::shared_ptr<UDP_Server> UDP_Server::create(Engine *engine,
                                               std::error_code &error) {
  auto ptr = std::make_shared<UDP_Server>(PrivateToken{}, engine);
  ptr->asyncListen(IPv6Endpoint{10000}, error);
  return ptr;
}

UDP_Server::UDP_Server(PrivateToken t, Engine *engine, ChannelMode mode,
                       UInt64 securityId, ProtocolVersions versions, UInt64 connId)
    : engine_{engine},
      mode_{mode},
      versions_{versions},
      socket_{engine->io()},
      connId_{connId},
      securityId_{securityId} {
  datagrams_.emplace_back();
}

UDP_Server::UDP_Server(PrivateToken t, Engine *engine)
    : engine_{engine},
      mode_{ChannelMode::Raw},
      versions_{ProtocolVersions::All},
      socket_{engine->io()},
      connId_{0},
      securityId_{0} {
  datagrams_.emplace_back();
}

UDP_Server::~UDP_Server() {
  if (!connMap_.empty()) {
    // By this point, all connections should have removed themselves. If any
    // exist, we need to shut them down.
    log::info(connMap_.size(), "UDP connections still exist!",
              std::make_pair("connid", connId_));

    // Safe way to iterate over connMap_ to close existing connections. N.B.
    // connections remove themselves from connMap_ (if they find themselves)
    // inside shutdown().
    auto iter = connMap_.begin();
    auto iterEnd = connMap_.end();
    while (iter != iterEnd) {
      auto conn = iter->second;
      iter = connMap_.erase(iter);
      conn->shutdown();
    }
  }

  if (connId_) {
    log::info("Stop listening on UDP", std::make_pair("connid", connId_));
  }
}

UInt64 UDP_Server::connect(const IPv6Endpoint &remoteEndpt,
                           UInt64 securityId,
                           ChannelListener::Factory factory,
                           std::error_code &error) {
  // Convert remoteEndpt to IPv6 address format if necessary.
  auto endpt = convertDestinationEndpoint(remoteEndpt, protocol_, error);
  if (error) {
    return 0;
  }

  auto conn = new UDP_Connection(this, mode_, securityId, versions_, factory);
  conn->connect(endpt);
  return conn->connectionId();
}

ofp::IPv6Endpoint UDP_Server::localEndpoint() const {
  asio::error_code err;
  return convertEndpoint<udp>(socket_.local_endpoint(err));
}

void UDP_Server::shutdown() {
  asio::error_code err;
  socket_.close(err);
}

void UDP_Server::add(UDP_Connection *conn) {
  auto result = connMap_.insert({conn->remoteEndpoint(), conn});
  if (!result.second) {
    auto existing = connMap_[conn->remoteEndpoint()];
    log::warning("UDP_Server::add: duplicate UDP connection ignored",
                 conn->remoteEndpoint(), existing->connectionId(),
                 std::make_pair("connid", conn->connectionId()));
  }
}

void UDP_Server::remove(UDP_Connection *conn) {
  auto iter = connMap_.find(conn->remoteEndpoint());
  if (iter == connMap_.end()) {
    log::warning("UDP_Server::remove: cannot find remote endpoint",
                 conn->remoteEndpoint(),
                 std::make_pair("connid", conn->connectionId()));
    return;
  }

  if (iter->second == conn) {
    connMap_.erase(iter);
  } else {
    log::warning("UDP_Server::remove: duplicate UDP connection ignored",
                 conn->remoteEndpoint(),
                 std::make_pair("connid", conn->connectionId()));
  }
}

UDP_Connection *UDP_Server::findConnection(const IPv6Endpoint &endpt) {
  auto iter = connMap_.find(endpt);
  if (iter != connMap_.end()) {
    return iter->second;
  }

  return nullptr;
}

void UDP_Server::send(udp::endpoint endpt, UInt64 connId, const void *data, size_t length) {
  assert(!datagrams_.empty());

  Datagram &datagram = datagrams_.back();
  datagram.write(data, length);
  datagram.setDestination(endpt);
  datagram.setConnectionId(connId);

  asyncSend();
}

#if 0
void UDP_Server::write(const void *data, size_t length) {
  assert(!datagrams_.empty());
  datagrams_.back().write(data, length);
}

void UDP_Server::flush(udp::endpoint endpt, UInt64 connId) {
  assert(!datagrams_.empty());

  Datagram &datagram = datagrams_.back();
  datagram.setDestination(endpt);
  datagram.setConnectionId(connId);

  asyncSend();
}
#endif //0

void UDP_Server::asyncListen(const IPv6Endpoint &localEndpt,
                             std::error_code &error) {
  listen(localEndpt, error);

  if (!error) {
    // Assign connection ID if necessary.
    if (connId_ == 0) {
      connId_ = engine_->assignConnectionId();
    }

    // Log message using the new local endpoint, if one was assigned.
    log::info("Start listening on UDP", localEndpoint(),
              std::make_pair("connid", connId_));

    asyncReceive();

  } else {
    connId_ = 0;
    log::error("Listen failed on UDP", localEndpt, error);
  }
}

void UDP_Server::listen(const IPv6Endpoint &localEndpt,
                        std::error_code &error) {
  // Handle case where IPv6 is not supported on this system.
  udp::endpoint endpt = convertEndpoint<udp>(localEndpt);
  asio::ip::address addr = endpt.address();

  socket_.open(endpt.protocol(), error);

  if (error == asio::error::address_family_not_supported && addr.is_v6() &&
      addr.is_unspecified()) {
    log::info("UDP_Server: IPv6 is not supported. Using IPv4.");
    endpt = udp::endpoint{udp::v4(), endpt.port()};
    if (socket_.open(endpt.protocol(), error)) return;
  }

  protocol_ = endpt.protocol();
  socket_.bind(endpt, error);
}

void UDP_Server::asyncReceive() {
  auto self(this->shared_from_this());

  buffer_.resize(MaxDatagramLength);

  socket_.async_receive_from(
      asio::buffer(buffer_.mutableData(), buffer_.size()),
      sender_, [this, self](const asio::error_code &err, size_t bytes_recvd) {
        if (err == asio::error::operation_aborted) return;

        if (err) {
          log::error("Error receiving datagram",
                     std::make_pair("connid", connId_), err);

        } else {
          buffer_.resize(bytes_recvd);
          datagramReceived();
        }

        asyncReceive();
      });
}

void UDP_Server::asyncSend() {
  auto self(this->shared_from_this());

  const Datagram &datagram = datagrams_.back();
  datagrams_.emplace_back();

  //log::trace("Write", datagram.connectionId(), datagram.data(),
  //           datagram.size());

  socket_.async_send_to(asio::buffer(datagram.data(), datagram.size()),
                        datagram.destination(),
                        [this, self, &datagram](const asio::error_code &err,
                                                size_t bytesTransferred) {
    if (err == asio::error::operation_aborted) {
      log::warning("UDP_Server::asyncSend: operation_aborted");
      return;
    }

    assert(&datagram == &datagrams_.front());

    if (err) {
      log::error("Error sending datagram to", datagram.destination(),
                 std::make_pair("connid", datagram.connectionId()), err);
    }
    datagrams_.pop_front();
  });
}

void UDP_Server::datagramReceived() {
  #if 0
  // If the message is an EchoRequest, reply immediately; it doesn't matter
  // if there is an existing connection or not.
  if (message_.type() == EchoRequest::type()) {
    auto request = EchoRequest::cast(&message_);
    if (request) {
      message_.mutableHeader()->setType(EchoReply::type());
      write(message_.data(), message_.size());
      flush(sender_, 0);
    } else {
      log::info("Invalid EchoRequest dropped.");
    }
    return;
  }
  #endif //0

  // Lookup sender_ to find an existing UDP_Connection. If it exists, dispatch
  // incoming message to that connection. If there is no related connection,
  // open a new connection.

  auto conn = findConnection(convertEndpoint<udp>(sender_));
  if (!conn) {
    // TODO(bfish): check if this UDP server allows incoming UDP connections...

    conn = new UDP_Connection(this, mode_, securityId_, versions_, nullptr);
    conn->accept(sender_);
  }

  conn->datagramReceived(buffer_.data(), buffer_.size());
  if (conn->isShutdown()) {
    delete conn;
  }
}
