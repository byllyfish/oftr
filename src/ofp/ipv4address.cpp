// Copyright 2014-present Bill Fisher. All rights reserved.

#include <arpa/inet.h>
#include "ofp/ipv4address.h"
#include "ofp/log.h"

using namespace ofp;

IPv4Address::IPv4Address(const ArrayType &a) : addr_(a) {
}

IPv4Address::IPv4Address(const std::string &s) {
  // TODO(bfish): this function should handle / notation. ie. "/24" should give
  // you a network mask 255.255.255.0. and a.b.c.d/24 should give you a.b.c.0

  if (!parse(s)) {
    addr_.fill(0);
  }
}

/// \returns Subnet mask of the specified length: /24 is 255.255.255.0
IPv4Address IPv4Address::mask(unsigned prefix) {
  assert(prefix <= 32U);

  IPv4Address result;

  unsigned d = std::min(4U, prefix / 8);
  unsigned r = prefix % 8;

  std::memset(result.addr_.data(), 0xFF, d);
  if (d < 4) {
    result.addr_[d] = UInt8_narrow_cast(0xFFU << (8 - r));
  }

  return result;
}

unsigned IPv4Address::prefix() const {
  // Count the number of 1 bits in the prefix. Assumes 1 bits are all
  // contiguous.

  unsigned result = 0;

  for (unsigned i = 0; i < Length; ++i) {
    if (addr_[i] != 0xFF) {
      // Count prefix bits in unfilled byte.
      unsigned v = addr_[i];
      while (v & 0x80) {
        v = (v << 1) & 0xFF;
        ++result;
      }
      return result;
    }
    result += 8;
  }

  return result;
}

bool IPv4Address::parse(const std::string &s) {
  int result = inet_pton(AF_INET, s.c_str(), addr_.data());
  return (result > 0);
}

void IPv4Address::clear() {
  std::memset(addr_.data(), 0, sizeof(addr_));
}

std::string IPv4Address::toString() const {
  char ipv4str[INET_ADDRSTRLEN] = {};
  const char *result =
      inet_ntop(AF_INET, addr_.data(), ipv4str, sizeof(ipv4str));
  return result ? ipv4str : "<inet_ntop_error4>";
}
