//  ===== ---- ofp/ipv6address.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the IPv6Address class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_IPV6ADDRESS_H
#define OFP_IPV6ADDRESS_H

#include "ofp/types.h"
#include "ofp/ipv4address.h"
#include <array>

namespace ofp { // <namespace ofp>

class IPv6Address {
public:
  enum {
    Length = 16
  };

  using ArrayType = std::array<UInt8, Length>;

  IPv6Address() : addr_{} {}
  IPv6Address(const IPv4Address &addr);
  explicit IPv6Address(const ArrayType &a);
  IPv6Address(const std::string &s);

  bool parse(const std::string &s);
  void clear();

  bool valid() const { return !IsMemFilled(addr_.data(), sizeof(addr_), '\0'); }

  bool isV4Mapped() const {
    return IsMemFilled(addr_.data(), 11, '\0') && (addr_[11] == 0xFF);
  }

  IPv4Address toV4() const {
    assert(isV4Mapped());
    IPv4Address v4;
    std::memcpy(&v4, &addr_[12], sizeof(v4));
    return v4;
  }

  std::string toString() const;

  ArrayType toArray() const { return addr_; }

  bool operator==(const IPv6Address &rhs) const { return addr_ == rhs.addr_; }

  bool operator!=(const IPv6Address &rhs) const { return !(*this == rhs); }

private:
  ArrayType addr_;
};

static_assert(IsStandardLayout<IPv6Address>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<IPv6Address>(),
              "Expected trivially copyable.");

std::ostream &operator<<(std::ostream &os, const IPv6Address &value);

} // </namespace ofp>

inline std::ostream &ofp::operator<<(std::ostream &os,
                                     const ofp::IPv6Address &value) {
  return os << value.toString();
}

#endif // OFP_IPV6ADDRESS_H
