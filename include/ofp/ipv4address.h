//  ===== ---- ofp/ipv4address.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the IPv4Address class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_IPV4ADDRESS_H
#define OFP_IPV4ADDRESS_H

#include "ofp/types.h"
#include "ofp/array.h"

namespace ofp { // <namespace ofp>

class IPv4Address {
public:
  enum {
    Length = 4
  };

  using ArrayType = std::array<UInt8, Length>;

  IPv4Address() : addr_{} {}
  explicit IPv4Address(const ArrayType &a);
  IPv4Address(const std::string &s);

  static IPv4Address mask(unsigned prefix);

  bool valid() const { return !IsMemFilled(addr_.data(), sizeof(addr_), '\0'); }

  unsigned prefix() const;
  bool isBroadcast() const {
    return IsMemFilled(addr_.data(), sizeof(addr_), '\xff');
  }

  void setAllOnes() { std::memset(addr_.data(), 0xFF, sizeof(addr_)); }

  bool parse(const std::string &s);
  void clear();

  std::string toString() const;

  const ArrayType &toArray() const { return addr_; }

  bool operator==(const IPv4Address &rhs) const { return addr_ == rhs.addr_; }

  bool operator!=(const IPv4Address &rhs) const { return !(*this == rhs); }

  bool fromString(const std::string &s);

private:
  ArrayType addr_;
};

static_assert(IsStandardLayout<IPv4Address>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<IPv4Address>(), "Expected trivially copyable.");

} // </namespace ofp>

namespace std { // <namespace std>

template <>
struct hash<ofp::IPv4Address> {
  size_t operator()(const ofp::IPv4Address &addr) const {
    return hash<ofp::IPv4Address::ArrayType>{}(addr.toArray());
  }
};

} // </namespace std>

#endif // OFP_IPV4ADDRESS_H
