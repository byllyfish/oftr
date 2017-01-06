// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/ipv4address.h"
#include <arpa/inet.h>
#include "ofp/log.h"

using namespace ofp;

static bool alternateParse(llvm::StringRef s, IPv4Address::ArrayType &addr);

IPv4Address::IPv4Address(const std::string &s) {
  if (!parse(s)) {
    clear();
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

#if !defined(NDEBUG) && defined(LIBOFP_TARGET_DARWIN)
  // Force consistency testing of alternate_parse on Mac OS X.
  ArrayType temp;
  bool alt_result = alternateParse(s, temp);
  assert((result > 0) == alt_result);
  if (result > 0) {
    assert(temp == addr_);
  }
#endif

  if (result == 0) {
    // inet_pton() on Linux does not accept zero-padded IPv4 addresses like
    // "127.000.000.001", which is accepted by the BSD implementation. Padded
    // IPv4 addresses are the default output format for tools like `tcpflow`.
    return alternateParse(s, addr_);
  }

  return (result > 0);
}

std::string IPv4Address::toString() const {
  std::string buf;              // FIXME(bfish) - common code...
  llvm::raw_string_ostream oss{buf};
  oss << *this;
  return oss.str();
}

static bool alternateParse(llvm::StringRef s, IPv4Address::ArrayType &addr) {
  using llvm::StringRef;
  size_t sp = 0;

  for (unsigned i = 0; i < 3; ++i) {
    size_t ep = s.find_first_not_of("0123456789", sp);
    if (ep == StringRef::npos || s[ep] != '.')
      return false;
    if (s.slice(sp, ep).getAsInteger(10, addr[i]))
      return false;
    sp = ep + 1;
  }

  if (sp == StringRef::npos ||
      s.find_first_not_of("0123456789", sp) != StringRef::npos)
    return false;

  if (s.slice(sp, StringRef::npos).getAsInteger(10, addr[3]))
    return false;

  return true;
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const IPv4Address &value) {
  char buf[INET_ADDRSTRLEN];
  const char *result = inet_ntop(AF_INET, value.addr_.data(), buf, sizeof(buf));
  return os << (result ? result : "<inet_ntop_error4>");
}

}  // namespace ofp
