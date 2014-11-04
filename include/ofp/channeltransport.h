// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_CHANNELTRANSPORT_H_
#define OFP_CHANNELTRANSPORT_H_

namespace ofp {

enum class ChannelTransport {
  None = 0,
  TCP_Plaintext,
  UDP_Plaintext,
  TCP_TLS,
  UDP_DTLS
};

// Specialize this function to return the channel transport for a specific
// socket type.
template <class SocketType>
constexpr ChannelTransport ToChannelTransport() {
  return ChannelTransport::None;
}

constexpr bool IsChannelTransportUDP(ChannelTransport transport) {
  return (transport == ChannelTransport::UDP_Plaintext) ||
         (transport == ChannelTransport::UDP_DTLS);
}

}  // namespace ofp

#endif  // OFP_CHANNELTRANSPORT_H_
