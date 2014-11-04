// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_SYS_TCP_CONNECTION_H_
#define OFP_SYS_TCP_CONNECTION_H_

#include "ofp/types.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/engine.h"
#include "ofp/message.h"
#include "ofp/sys/connection.h"
#include "ofp/driver.h"
#include "ofp/sys/buffered.h"
#include "ofp/sys/handler_allocator.h"

namespace ofp {

template <>
constexpr ChannelTransport ToChannelTransport<sys::PlaintextSocket>() {
  return ChannelTransport::TCP_Plaintext;
}

template <>
constexpr ChannelTransport ToChannelTransport<sys::EncryptedSocket>() {
  return ChannelTransport::TCP_TLS;
}

namespace sys {

OFP_BEGIN_IGNORE_PADDING

template <class SocketType>
class TCP_Connection
    : public std::enable_shared_from_this<TCP_Connection<SocketType>>,
      public Connection {
 public:
  TCP_Connection(Engine *engine, ChannelMode mode, UInt64 securityId,
                 ProtocolVersions versions, ChannelListener::Factory factory);
  TCP_Connection(Engine *engine, tcp::socket socket, ChannelMode mode,
                 UInt64 securityId, ProtocolVersions versions,
                 ChannelListener::Factory factory);
  ~TCP_Connection();

  void asyncConnect(
      const IPv6Endpoint &remoteEndpt,
      std::function<void(Channel *, std::error_code)> resultHandler);
  void asyncAccept();

  ChannelTransport transport() const override {
    return ToChannelTransport<SocketType>();
  }
  IPv6Endpoint remoteEndpoint() const override;
  IPv6Endpoint localEndpoint() const override;

  void write(const void *data, size_t length) override {
    socket_.buf_write(data, length);
  }

  void flush() override;
  void shutdown() override;

 private:
  Message message_;
  Buffered<SocketType> socket_;
  std::chrono::steady_clock::time_point latestActivity_;
  handler_allocator allocator_;
  bool isChannelUp_ = false;

  void channelUp();
  void channelDown();

  void asyncReadHeader();
  void asyncReadMessage(size_t length);
  void asyncWrite();
  void asyncHandshake(bool isClient);

  void updateLatestActivity() {
    latestActivity_ = std::chrono::steady_clock::now();
  }
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#include "ofp/sys/tcp_connection.ipp"

#endif  // OFP_SYS_TCP_CONNECTION_H_
