// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_CHANNELTRANSPORT_H_
#define OFP_CHANNELTRANSPORT_H_

namespace ofp {

enum class ChannelTransport {
  None = 0,
  TCP_Plaintext,
  TCP_TLS
};

namespace sys {

// Specialize this function to return the channel transport for a specific
// socket type.
template <class SocketType>
constexpr ChannelTransport ToChannelTransport() {
  return ChannelTransport::None;
}

}  // namespace sys
}  // namespace ofp

#endif  // OFP_CHANNELTRANSPORT_H_
