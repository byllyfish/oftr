// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/udp_connection.h"
#include "ofp/defaulthandshake.h"
#include "ofp/sys/udp_server.h"
#include "ofp/log.h"
#include "ofp/sys/engine.h"
#include "ofp/sys/plaintext_adapter.h"

namespace ofp {
namespace sys {

namespace detail {

template <class Adapter>
inline SSL_CTX *udpContext(UDP_Server *server, UInt64 securityId) {
  assert(securityId != 0);
  Identity *identity = server->engine()->findIdentity(securityId);
  log::fatal_if_null(identity, LOG_LINE());
  return identity->dtlsContext();
}

template <>
inline SSL_CTX *udpContext<Plaintext_Adapter>(UDP_Server *server,
                                              UInt64 securityId) {
  assert(securityId == 0);
  return nullptr;
}

}  // namespace detail

template <class AdapterType>
UDP_Connection<AdapterType>::UDP_Connection(UDP_Server *server,
                                            ChannelMode mode, UInt64 securityId,
                                            ProtocolVersions versions,
                                            ChannelListener::Factory factory)
    : Connection{server->engine(),
                 new DefaultHandshake{this, mode, versions, factory}},
      server_{server},
      dtls_{detail::udpContext<AdapterType>(server, securityId), sendCallback,
            receiveCallback, this} {
  UInt16 newFlags = (securityId == 0)
                        ? Connection::kManualDelete
                        : (Connection::kManualDelete | kRequiresHandshake);
  setFlags(flags() | newFlags);
}

template <class AdapterType>
UDP_Connection<AdapterType>::~UDP_Connection() {
  if (connectionId()) {
    channelDown();
    log::info("Close UDP connection", std::make_pair("connid", connectionId()));
    server_->remove(this);
  }
}

template <class AdapterType>
void UDP_Connection<AdapterType>::connect(const udp::endpoint &remoteEndpt) {
  remoteEndpt_ = remoteEndpt;
  server_->add(this);

  Identity::beforeHandshake(this, dtls_.native_handle(), true);
  dtls_.connect();

  log::info("Establish UDP connection", localEndpoint(), "-->",
            remoteEndpoint(), std::make_pair("connid", connectionId()));

  if (!(flags() & Connection::kRequiresHandshake)) {
    channelUp();
  }
}

template <class AdapterType>
void UDP_Connection<AdapterType>::accept(const udp::endpoint &remoteEndpt) {
  remoteEndpt_ = remoteEndpt;
  server_->add(this);

  Identity::beforeHandshake(this, dtls_.native_handle(), false);
  dtls_.accept();

  log::info("Accept UDP connection", localEndpoint(), "<--", remoteEndpoint(),
            std::make_pair("connid", connectionId()));

  if (!(flags() & Connection::kRequiresHandshake)) {
    channelUp();
  }
}

template <class AdapterType>
IPv6Endpoint UDP_Connection<AdapterType>::remoteEndpoint() const {
  return convertEndpoint<udp>(remoteEndpt_);
}

template <class AdapterType>
IPv6Endpoint UDP_Connection<AdapterType>::localEndpoint() const {
  return server_->localEndpoint();
}

template <class AdapterType>
void UDP_Connection<AdapterType>::write(const void *data, size_t length) {
  buffer_.add(data, length);
}

template <class AdapterType>
void UDP_Connection<AdapterType>::flush() {
  log::trace_msg("Write", connectionId(), buffer_.data(), buffer_.size());
  dtls_.sendDatagram(buffer_.data(), buffer_.size());
  buffer_.clear();
}

template <class AdapterType>
void UDP_Connection<AdapterType>::shutdown() {
  setFlags(flags() | Connection::kShutdownCalled);
}

template <class AdapterType>
void UDP_Connection<AdapterType>::datagramReceived(const void *data,
                                                   size_t length) {
  dtls_.datagramReceived(data, length);

  if (!(flags() & Connection::kHandshakeDone) && dtls_.isHandshakeDone()) {
    std::error_code err;
    Identity::afterHandshake(this, dtls_.native_handle(), err);
    if (!err) {
      channelUp();
    }
  }
}

template <class AdapterType>
void UDP_Connection<AdapterType>::sendCiphertext(const void *data,
                                                 size_t length) {
  server_->send(remoteEndpt_, connectionId(), data, length);
}

template <class AdapterType>
void UDP_Connection<AdapterType>::receivePlaintext(const void *data,
                                                   size_t length) {
  Message message(data, length);
  message.setSource(this);
  postMessage(&message);
}

template <class AdapterType>
void UDP_Connection<AdapterType>::sendCallback(const void *data, size_t length,
                                               void *userData) {
  // log::debug("sendCallback", ByteRange{data, length});
  UDP_Connection *conn = static_cast<UDP_Connection *>(userData);
  conn->sendCiphertext(data, length);
}

template <class AdapterType>
void UDP_Connection<AdapterType>::receiveCallback(const void *data,
                                                  size_t length,
                                                  void *userData) {
  // log::debug("receiveCallback", ByteRange{data, length});
  UDP_Connection *conn = static_cast<UDP_Connection *>(userData);
  conn->receivePlaintext(data, length);
}

}  // namespace sys
}  // namespace ofp
