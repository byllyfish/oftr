// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_ASIO_UTILS_H_
#define OFP_SYS_ASIO_UTILS_H_

#include "ofp/sys/asio_openssl_init.h"
#include <asio.hpp>
#include <asio/ssl.hpp>

#include "ofp/log.h"
#include "ofp/ipv6endpoint.h"
#include "ofp/sys/plaintext.h"

namespace ofp {
namespace sys {

using tcp = asio::ip::tcp;
using udp = asio::ip::udp;

using PlaintextSocket = Plaintext<tcp::socket>;
using EncryptedSocket = asio::ssl::stream<tcp::socket>;

/// Convert an asio::ip::address into an ofp::IPv6Address.
inline IPv6Address makeIPv6Address(const asio::ip::address &addr) {
  if (addr.is_v6()) {
    asio::ip::address_v6 addr6 = addr.to_v6();
    return IPv6Address{addr6.to_bytes()};
  } else {
    assert(addr.is_v4());
    asio::ip::address_v4 addr4 = addr.to_v4();
    IPv4Address v4{addr4.to_bytes()};
    return IPv6Address{v4};
  }
}

/// Create an asio::<proto>::endpoint from an ofp::IPv6Address and port number.
template <class Proto>
inline typename Proto::endpoint makeEndpoint(const IPv6Address &addr,
                                             UInt16 port) {
  using Endpoint = typename Proto::endpoint;

  if (!addr.valid()) {
    return Endpoint{Proto::v6(), port};
  } else if (addr.isV4Mapped()) {
    asio::ip::address_v4 v4{addr.toV4().toArray()};
    return Endpoint{v4, port};
  } else {
    asio::ip::address_v6 v6{addr.toArray()};
    return Endpoint{v6, port};
  }
}

template <class Proto>
inline typename Proto::endpoint convertEndpoint(const IPv6Endpoint &endpt) {
  return makeEndpoint<Proto>(endpt.address(), endpt.port());
}

template <class Proto>
inline IPv6Endpoint convertEndpoint(const typename Proto::endpoint &endpt) {
  return IPv6Endpoint{makeIPv6Address(endpt.address()), endpt.port()};
}

inline udp::endpoint convertDestinationEndpoint(const IPv6Endpoint &endpt,
                                                udp proto,
                                                std::error_code &error) {
  IPv6Address addr = endpt.address();
  UInt16 port = endpt.port();

  if (addr.valid() && port != 0) {
    if (proto == udp::v6()) {
      asio::ip::address_v6 v6{addr.toArray()};
      return udp::endpoint{v6, port};
    } else if (proto == udp::v4() && addr.isV4Mapped()) {
      asio::ip::address_v4 v4{addr.toV4().toArray()};
      return udp::endpoint{v4, port};
    }
  }

  error = std::make_error_code(std::errc::invalid_argument);

  return udp::endpoint{};
}

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_ASIO_UTILS_H_
