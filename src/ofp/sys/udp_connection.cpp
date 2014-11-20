// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/sys/udp_connection.h"
#include "ofp/defaulthandshake.h"
#include "ofp/sys/udp_server.h"
#include "ofp/log.h"
#include "ofp/sys/engine.h"

using namespace ofp;
using namespace sys;

inline asio::ssl::context *sslContext(UDP_Server *server, UInt64 securityId) {
  assert(securityId != 0);
  return log::fatal_if_null(server->engine()->securityContext(securityId));
}


UDP_Connection::UDP_Connection(UDP_Server *server, ChannelMode mode, UInt64 securityId,
                               ProtocolVersions versions,
                               ChannelListener::Factory factory)
    : Connection{server->engine(),
                 new DefaultHandshake{this, mode, versions, factory}},
      server_{server}, dtls_{sslContext(server, securityId), sendCallback, receiveCallback, this} {
  UInt16 newFlags = (securityId == 0) ? Connection::kManualDelete : (Connection::kManualDelete | kRequiresHandshake);
  setFlags(flags() | newFlags);
}

UDP_Connection::~UDP_Connection() {
  if (connectionId()) {
    channelDown();
    log::info("Close UDP connection", std::make_pair("connid", connectionId()));
    server_->remove(this);
  }
}

void UDP_Connection::connect(const udp::endpoint &remoteEndpt) {
  remoteEndpt_ = remoteEndpt;
  server_->add(this);

  Identity::beforeHandshake(this, dtls_.native_handle(), true);
  dtls_.connect();

  log::info("Establish UDP connection", localEndpoint(), "-->",
            remoteEndpoint(), std::make_pair("connid", connectionId()));
}

void UDP_Connection::accept(const udp::endpoint &remoteEndpt) {
  remoteEndpt_ = remoteEndpt;
  server_->add(this);

  Identity::beforeHandshake(this, dtls_.native_handle(), false);
  dtls_.accept();

  log::info("Accept UDP connection", localEndpoint(), "<--", remoteEndpoint(),
            std::make_pair("connid", connectionId()));
}

IPv6Endpoint UDP_Connection::remoteEndpoint() const {
  return convertEndpoint<udp>(remoteEndpt_);
}

IPv6Endpoint UDP_Connection::localEndpoint() const {
  return server_->localEndpoint();
}

void UDP_Connection::write(const void *data, size_t length) {
  buffer_.add(data, length);
  //server_->write(data, length);
}

void UDP_Connection::flush() { 
  log::trace("Write", connectionId(), buffer_.data(), buffer_.size());
  dtls_.sendDatagram(buffer_.data(), buffer_.size());
  buffer_.clear();
}

void UDP_Connection::shutdown() { 
  setFlags(flags() | Connection::kShutdownCalled);
 }


void UDP_Connection::datagramReceived(const void *data, size_t length) {
  dtls_.datagramReceived(data, length);

  if (!(flags() & Connection::kHandshakeDone) && dtls_.isHandshakeDone()) {
    std::error_code err;
    Identity::afterHandshake(this, dtls_.native_handle(), err);
    log::info("UDP_Connection::afterHandshake error", err);
    channelUp();
  }
}

void UDP_Connection::sendCiphertext(const void *data, size_t length) {
  server_->send(remoteEndpt_, connectionId(), data, length);
}

void UDP_Connection::receivePlaintext(const void *data, size_t length) {
  Message message(data, length);
  message.setSource(this);
  postMessage(&message);
}

void UDP_Connection::sendCallback(const void *data, size_t length, void *userData) {
  log::debug("sendCallback", ByteRange{data, length});
  UDP_Connection *conn = reinterpret_cast<UDP_Connection*>(userData);
  conn->sendCiphertext(data, length);
}

void UDP_Connection::receiveCallback(const void *data, size_t length, void *userData) {
  log::debug("receiveCallback", ByteRange{data, length});
  UDP_Connection *conn = reinterpret_cast<UDP_Connection*>(userData);
  conn->receivePlaintext(data, length);
}

