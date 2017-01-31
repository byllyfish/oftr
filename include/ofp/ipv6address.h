// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_IPV6ADDRESS_H_
#define OFP_IPV6ADDRESS_H_

#include <array>
#include "ofp/ipv4address.h"

namespace ofp {

class IPv6Address {
 public:
  enum { Length = 16 };

  using ArrayType = std::array<UInt8, Length>;

  IPv6Address() : addr_{} {}
  /* implicit NOLINT */ IPv6Address(const IPv4Address &addr);
  explicit IPv6Address(const ArrayType &a) : addr_(a) {}
  /* implicit NOLINT */ IPv6Address(const std::string &s);

  /// \returns zone_id for link-local address.
  UInt32 zone() const;

  bool parse(const std::string &s, bool parseIPv4 = true);
  void clear() { addr_.fill(0); }

  bool valid() const { return !IsMemFilled(addr_.data(), sizeof(addr_), '\0'); }

  bool isV4Mapped() const {
    return IsMemFilled(addr_.data(), 10, '\0') && (addr_[10] == 0xFF) &&
           (addr_[11] == 0xFF);
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
  void outputV6(llvm::raw_ostream &os) const;

  const ArrayType &toArray() const { return addr_; }

  bool operator==(const IPv6Address &rhs) const { return addr_ == rhs.addr_; }
  bool operator!=(const IPv6Address &rhs) const { return addr_ != rhs.addr_; }
  bool operator<(const IPv6Address &rhs) const { return addr_ < rhs.addr_; }
  bool operator>(const IPv6Address &rhs) const { return addr_ > rhs.addr_; }
  bool operator<=(const IPv6Address &rhs) const { return addr_ <= rhs.addr_; }
  bool operator>=(const IPv6Address &rhs) const { return addr_ >= rhs.addr_; }

 private:
  ArrayType addr_;

  void setZone(UInt32 zone);

  bool parseIPv6Address(const std::string &s);
  bool parseIPv4Address(const std::string &s);

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                       const IPv6Address &value);
};

static_assert(sizeof(IPv6Address) == 16, "Unexpected size.");
static_assert(alignof(IPv6Address) == 1, "Unexpected alignment.");
static_assert(IsStandardLayout<IPv6Address>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<IPv6Address>(),
              "Expected trivially copyable.");

}  // namespace ofp

namespace std {

template <>
struct hash<ofp::IPv6Address> {
  size_t operator()(const ofp::IPv6Address &addr) const {
    return ofp::hash::MurmurHash32(&addr);
  }
};

}  // namespace std

#endif  // OFP_IPV6ADDRESS_H_
