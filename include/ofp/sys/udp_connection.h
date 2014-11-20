// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_SYS_UDP_CONNECTION_H_
#define OFP_SYS_UDP_CONNECTION_H_

#include "ofp/sys/connection.h"
#include "ofp/driver.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/protocolversions.h"
#include "ofp/sys/dtls_adapter.h"
#include "ofp/bytelist.h"

namespace ofp {
namespace sys {

class UDP_Server;

OFP_BEGIN_IGNORE_PADDING

class UDP_Connection : public Connection {
 public:
  UDP_Connection(UDP_Server *server, ChannelMode mode, UInt64 securityId,
                 ProtocolVersions versions, ChannelListener::Factory factory);
  ~UDP_Connection();

  void connect(const udp::endpoint &remoteEndpt);
  void accept(const udp::endpoint &remoteEndpt);

  void write(const void *data, size_t length) override;
  void flush() override;
  void shutdown() override;

  ChannelTransport transport() const override {
    return ChannelTransport::UDP_Plaintext;
  }

  IPv6Endpoint remoteEndpoint() const override;
  IPv6Endpoint localEndpoint() const override;

  void datagramReceived(const void *data, size_t length);
  bool isShutdown() const { return isShutdown_; }

  bool shutdownRequiresManualDelete() const override { return true; }
  
 private:
  UDP_Server *server_;
  DTLS_Adapter dtls_;
  udp::endpoint remoteEndpt_;
  ByteList buffer_;
  bool isShutdown_ = false;
  bool isHandshakeDone_ = false;

  void channelUp();

  void sendCiphertext(const void *data, size_t length);
  void receivePlaintext(const void *data, size_t length);

  static void sendCallback(const void *data, size_t length, void *userData);
  static void receiveCallback(const void *data, size_t length, void *userData);
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_UDP_CONNECTION_H_
