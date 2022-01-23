// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_IPV4ADDRESS_H_
#define OFP_IPV4ADDRESS_H_

#include <array>

#include "ofp/hash.h"

namespace ofp {

class IPv4Address {
 public:
  enum { Length = 4 };

  using ArrayType = std::array<UInt8, Length>;

  IPv4Address() : addr_{} {}
  explicit IPv4Address(const ArrayType &a) : addr_(a) {}
  /* implicit NOLINT */ IPv4Address(llvm::StringRef s);

  static IPv4Address mask(unsigned prefix);

  bool valid() const { return !IsMemFilled(addr_.data(), sizeof(addr_), '\0'); }

  unsigned prefix() const;
  bool isBroadcast() const {
    return IsMemFilled(addr_.data(), sizeof(addr_), '\xff');
  }

  void setAllOnes() { addr_.fill(0xFF); }

  bool parse(llvm::StringRef s);
  void clear() { addr_.fill(0); }

  std::string toString() const { return detail::ToString(*this); }

  const ArrayType &toArray() const { return addr_; }

  bool operator==(const IPv4Address &rhs) const { return addr_ == rhs.addr_; }
  bool operator!=(const IPv4Address &rhs) const { return addr_ != rhs.addr_; }
  bool operator<(const IPv4Address &rhs) const { return addr_ < rhs.addr_; }
  bool operator>(const IPv4Address &rhs) const { return addr_ > rhs.addr_; }
  bool operator<=(const IPv4Address &rhs) const { return addr_ <= rhs.addr_; }
  bool operator>=(const IPv4Address &rhs) const { return addr_ >= rhs.addr_; }

 private:
  ArrayType addr_;

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                       const IPv4Address &value);
};

static_assert(sizeof(IPv4Address) == 4, "Unexpected size");
static_assert(alignof(IPv4Address) == 1, "Unexpected alignment");
static_assert(IsStandardLayout<IPv4Address>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<IPv4Address>(),
              "Expected trivially copyable.");

}  // namespace ofp

namespace std {

template <>
struct hash<ofp::IPv4Address> {
  size_t operator()(const ofp::IPv4Address &addr) const {
    return ofp::hash::MurmurHash32(&addr);
  }
};

}  // namespace std

#endif  // OFP_IPV4ADDRESS_H_
