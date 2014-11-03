//  ===== ---- ofp/ipv6endpoint.h --------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the IPv6Endpoint class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_IPV6ENDPOINT_H_
#define OFP_IPV6ENDPOINT_H_

#include <istream>
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

std::istream &operator>>(std::istream &is, IPv6Endpoint &value);
std::ostream &operator<<(std::ostream &os, const IPv6Endpoint &value);

inline std::istream &operator>>(std::istream &is, IPv6Endpoint &value) {
  std::string str;
  is >> str;
  if (!value.parse(str)) {
    is.setstate(std::ios::failbit);
  }
  return is;
}

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
