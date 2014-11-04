// Copyright 2014-present Bill Fisher. All rights reserved.

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

  bool parse(const std::string &s);
  void clear();

  bool valid() const { return port_ != 0; }
  const IPv6Address &address() const { return addr_; }
  UInt16 port() const { return port_; }

  void setAddress(const IPv6Address &addr) { addr_ = addr; }
  void setPort(UInt16 port) { port_ = port; }

  std::string toString() const;

  bool operator==(const IPv6Endpoint &rhs) const {
    return port_ == rhs.port_ && addr_ == rhs.addr_;
  }
  bool operator!=(const IPv6Endpoint &rhs) const { return !(*this == rhs); }

 private:
  IPv6Address addr_;
  UInt16 port_ = 0;
};

std::ostream &operator<<(std::ostream &os, const IPv6Endpoint &value);

inline std::ostream &operator<<(std::ostream &os, const IPv6Endpoint &value) {
  return os << value.toString();
}

}  // namespace ofp

namespace std {

template <>
struct hash<ofp::IPv6Endpoint> {
  size_t operator()(const ofp::IPv6Endpoint &endpt) const {
    return hash<ofp::IPv6Address>{}(endpt.address()) ^
           hash<unsigned>{}(endpt.port());
  }
};

}  // namespace std

#endif  // OFP_IPV6ENDPOINT_H_
