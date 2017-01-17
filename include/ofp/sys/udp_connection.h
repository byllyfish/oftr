// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_UDP_CONNECTION_H_
#define OFP_SYS_UDP_CONNECTION_H_

#include "ofp/bytelist.h"
#include "ofp/driver.h"
#include "ofp/protocolversions.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/connection.h"

namespace ofp {

template <>
constexpr ChannelTransport ToChannelTransport<sys::Plaintext_Adapter>() {
  return ChannelTransport::UDP_Plaintext;
}

template <>
constexpr ChannelTransport ToChannelTransport<sys::DTLS_Adapter>() {
  return ChannelTransport::UDP_DTLS;
}

namespace sys {

class UDP_Server;

OFP_BEGIN_IGNORE_PADDING

template <class AdapterType>
class UDP_Connection : public Connection {
 public:
  UDP_Connection(UDP_Server *server, ChannelOptions options, UInt64 securityId,
                 ProtocolVersions versions, ChannelListener::Factory factory);
  ~UDP_Connection();

  void connect(const udp::endpoint &remoteEndpt);
  void accept(const udp::endpoint &remoteEndpt);

  void write(const void *data, size_t length) override;
  void flush() override;
  void shutdown() override;

  ChannelTransport transport() const override {
    return ToChannelTransport<AdapterType>();
  }

  IPv6Endpoint remoteEndpoint() const override;
  IPv6Endpoint localEndpoint() const override;

  void datagramReceived(const void *data, size_t length) override;

 private:
  UDP_Server *server_;
  AdapterType dtls_;
  udp::endpoint remoteEndpt_;
  ByteList buffer_;

  void sendCiphertext(const void *data, size_t length);
  void receivePlaintext(const void *data, size_t length);

  static void sendCallback(const void *data, size_t length, void *userData);
  static void receiveCallback(const void *data, size_t length, void *userData);
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#include "ofp/sys/udp_connection.ipp"

#endif  // OFP_SYS_UDP_CONNECTION_H_
