//  ===== ---- ofp/enetaddress.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the EnetAddress class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ENETADDRESS_H
#define OFP_ENETADDRESS_H

#include "ofp/types.h"
#include "ofp/array.h"
#include "ofp/log.h"

namespace ofp { // <namespace ofp>

class EnetAddress {
public:
  enum {
    Length = 6
  };

  using ArrayType = std::array<UInt8, Length>;

  EnetAddress() : addr_{} {}
  EnetAddress(const std::string &s);

  bool parse(const std::string &s);

  bool valid() const { return !IsMemFilled(addr_.data(), sizeof(addr_), '\0'); }

  bool isMulticast() const { return (addr_[0] & 0x01); }
  bool isBroadcast() const {
    return IsMemFilled(addr_.data(), sizeof(addr_), '\xff');
  }
  void setAllOnes() { std::memset(addr_.data(), 0xFF, sizeof(addr_)); }

  void clear() { std::memset(addr_.data(), 0, sizeof(addr_)); }

  std::string toString() const;

  const ArrayType &toArray() const { return addr_; }

  bool operator==(const EnetAddress &rhs) const { return addr_ == rhs.addr_; }

  bool operator!=(const EnetAddress &rhs) const { return !(*this == rhs); }

private:
  ArrayType addr_;
};

static_assert(sizeof(EnetAddress) == 6, "Unexpected size.");
static_assert(IsStandardLayout<EnetAddress>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<EnetAddress>(),
              "Expected trivially copyable.");

} // </namespace ofp>

namespace std { // <namespace std>
template <>
struct hash<ofp::EnetAddress> {
  size_t operator()(const ofp::EnetAddress &rhs) const {
    std::hash<ofp::EnetAddress::ArrayType> hasher;
    return hasher(rhs.toArray());
  }
};
} // </namespace std>

#endif // OFP_ENETADDRESS_H
