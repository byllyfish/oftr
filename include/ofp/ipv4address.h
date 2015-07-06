// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_IPV4ADDRESS_H_
#define OFP_IPV4ADDRESS_H_

#include "ofp/types.h"
#include "ofp/array.h"

namespace ofp {

class IPv4Address {
 public:
  enum { Length = 4 };

  using ArrayType = std::array<UInt8, Length>;

  IPv4Address() : addr_{} {}
  explicit IPv4Address(const ArrayType &a);
  /* implicit NOLINT */ IPv4Address(const std::string &s);

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

static_assert(sizeof(IPv4Address) == 4, "Unexpected size");
static_assert(alignof(IPv4Address) == 1, "Unexpected alignment");
static_assert(IsStandardLayout<IPv4Address>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<IPv4Address>(),
              "Expected trivially copyable.");

inline std::ostream &operator<<(std::ostream &os, const IPv4Address &value) {
  return os << value.toString();
}

}  // namespace ofp

namespace std {

template <>
struct hash<ofp::IPv4Address> {
  size_t operator()(const ofp::IPv4Address &addr) const {
    std::hash<ofp::IPv4Address::ArrayType> h;
    return h(addr.toArray());
  }
};

}  // namespace std

#endif  // OFP_IPV4ADDRESS_H_
