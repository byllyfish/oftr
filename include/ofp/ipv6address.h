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

#ifndef OFP_IPV6ADDRESS_H_
#define OFP_IPV6ADDRESS_H_

#include "ofp/types.h"
#include "ofp/ipv4address.h"
#include "ofp/array.h"

namespace ofp {

class IPv6Address {
public:
  enum {
    Length = 16
  };

  using ArrayType = std::array<UInt8, Length>;

  IPv6Address() : addr_{} {}
  /* implicit NOLINT */ IPv6Address(const IPv4Address &addr);
  explicit IPv6Address(const ArrayType &a);
  /* implicit NOLINT */ IPv6Address(const std::string &s);

  /// \returns zone_id for link-local address.
  UInt32 zone() const;

  bool parse(const std::string &s);
  void clear();

  bool valid() const { return !IsMemFilled(addr_.data(), sizeof(addr_), '\0'); }

  bool isV4Mapped() const {
    return IsMemFilled(addr_.data(), 10, '\0') && (addr_[10] == 0xFF) && (addr_[11] == 0xFF);
  }

  /// \returns true if address is link-local (fe80::/10)
  bool isLinkLocal() const {
    return (addr_[0] == 0xFE) && ((addr_[1] & 0xC0) == 0x80);
  }

  IPv4Address toV4() const {
    assert(isV4Mapped());
    IPv4Address v4;
    std::memcpy(&v4, &addr_[12], sizeof(v4));
    return v4;
  }

  std::string toString() const;

  const ArrayType &toArray() const { return addr_; }

  bool operator==(const IPv6Address &rhs) const { return addr_ == rhs.addr_; }

  bool operator!=(const IPv6Address &rhs) const { return !(*this == rhs); }

private:
  ArrayType addr_;

  void setZone(UInt32 zone);
  
  bool parseIPv6Address(const std::string &s);
  bool parseIPv4Address(const std::string &s);
};

static_assert(IsStandardLayout<IPv6Address>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<IPv6Address>(),
              "Expected trivially copyable.");

std::ostream &operator<<(std::ostream &os, const IPv6Address &value);

inline std::ostream &operator<<(std::ostream &os, const IPv6Address &value) {
  return os << value.toString();
}

}  // namespace ofp

namespace std {

template <>
struct hash<ofp::IPv6Address> {
  size_t operator()(const ofp::IPv6Address &addr) const {
    return hash<ofp::IPv6Address::ArrayType>{}(addr.toArray());
  }
};

}  // namespace std

#endif  // OFP_IPV6ADDRESS_H_
