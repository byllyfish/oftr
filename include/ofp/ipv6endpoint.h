// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_IPV6ENDPOINT_H_
#define OFP_IPV6ENDPOINT_H_

#include "ofp/ipv6address.h"

namespace ofp {

class IPv6Endpoint {
 public:
  IPv6Endpoint() = default;
  IPv6Endpoint(const IPv6Address &addr, UInt16 port)
      : addr_{addr}, port_{port} {}
  IPv6Endpoint(const std::string &addr, UInt16 port)
      : addr_{addr}, port_{port} {}
  explicit IPv6Endpoint(UInt16 port) : addr_{}, port_{port} {}
  explicit IPv6Endpoint(const std::string &s);

  bool parse(const std::string &s);
  void clear();

  bool valid() const { return port_ != 0; }
  const IPv6Address &address() const { return addr_; }
  UInt16 port() const { return port_; }

  void setAddress(const IPv6Address &addr) { addr_ = addr; }
  void setPort(UInt16 port) { port_ = port; }

  std::string toString() const;

  bool operator==(const IPv6Endpoint &rhs) const {
    return addr_ == rhs.addr_ && port_ == rhs.port_;
  }
  bool operator!=(const IPv6Endpoint &rhs) const {
    return addr_ != rhs.addr_ || port_ != rhs.port_;
  }
  bool operator<(const IPv6Endpoint &rhs) const {
    return addr_ < rhs.addr_ || (addr_ == rhs.addr_ && port_ < rhs.port_);
  }
  bool operator>(const IPv6Endpoint &rhs) const {
    return addr_ > rhs.addr_ || (addr_ == rhs.addr_ && port_ > rhs.port_);
  }
  bool operator<=(const IPv6Endpoint &rhs) const {
    return addr_ < rhs.addr_ || (addr_ == rhs.addr_ && port_ <= rhs.port_);
  }
  bool operator>=(const IPv6Endpoint &rhs) const {
    return addr_ > rhs.addr_ || (addr_ == rhs.addr_ && port_ >= rhs.port_);
  }

 private:
  IPv6Address addr_;
  UInt16 port_ = 0;

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const IPv6Endpoint &value);
};

static_assert(alignof(IPv6Endpoint) == 2, "Unexpected alignment");
static_assert(sizeof(IPv6Endpoint) == 18, "Unexpected size");
static_assert(IsStandardLayout<IPv6Endpoint>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<IPv6Endpoint>(),
              "Expected trivially copyable.");

inline std::ostream &operator<<(std::ostream &os, const IPv6Endpoint &value) {
  return os << value.toString();
}

}  // namespace ofp

namespace std {

template <>
struct hash<ofp::IPv6Endpoint> {
  size_t operator()(const ofp::IPv6Endpoint &endpt) const {
    return ofp::hash::MurmurHash32(&endpt);
  }
};

}  // namespace std

#endif  // OFP_IPV6ENDPOINT_H_
