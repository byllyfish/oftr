// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/ipv4address.h"
#include <asio/detail/socket_ops.hpp>  // for asio's inet_pton, inet_ntop
#include "ofp/log.h"

using namespace ofp;

static const size_t kMaxAddrStrLen = asio::detail::max_addr_v4_str_len;
static_assert(kMaxAddrStrLen >= 16, "Unexpectedly small buffer");

IPv4Address::IPv4Address(llvm::StringRef s) {
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

bool IPv4Address::parse(llvm::StringRef s) {
  if (s.size() > kMaxAddrStrLen)
    return false;

  char buf[kMaxAddrStrLen + 1];
  std::memcpy(buf, s.data(), s.size());
  buf[s.size()] = 0;

  std::error_code err;
  int result = asio::detail::socket_ops::inet_pton(ASIO_OS_DEF(AF_INET), buf,
                                                   addr_.data(), 0, err);

  if (result == 0) {
    // inet_pton() on Linux does not accept zero-padded IPv4 addresses like
    // "127.000.000.001", which is accepted by the BSD implementation. Padded
    // IPv4 addresses are the default output format for tools like `tcpflow`.
    return alternateParse(s, addr_);
  }

  return (result > 0);
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const IPv4Address &value) {
  std::error_code err;
  char buf[kMaxAddrStrLen];
  const char *result = asio::detail::socket_ops::inet_ntop(
      ASIO_OS_DEF(AF_INET), value.addr_.data(), buf, sizeof(buf), 0, err);
  if (!result) {
    return os << "<inet_ntop_error4>";
  }
  return os << result;
}

}  // namespace ofp
