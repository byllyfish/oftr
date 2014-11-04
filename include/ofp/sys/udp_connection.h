// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_SYS_UDP_CONNECTION_H_
#define OFP_SYS_UDP_CONNECTION_H_

#include "ofp/sys/connection.h"
#include "ofp/driver.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/protocolversions.h"

namespace ofp {
namespace sys {

class UDP_Server;

OFP_BEGIN_IGNORE_PADDING

class UDP_Connection : public Connection {
 public:
  UDP_Connection(UDP_Server *server, ChannelMode mode,
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

 private:
  UDP_Server *server_;
  udp::endpoint remoteEndpt_;

  void channelUp();
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_UDP_CONNECTION_H_
