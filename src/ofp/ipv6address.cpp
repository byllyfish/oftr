// Copyright 2014-present Bill Fisher. All rights reserved.

#include <arpa/inet.h>
#include "ofp/ipv6address.h"
#include "ofp/log.h"
#include "ofp/byteorder.h"

using namespace ofp;

IPv6Address::IPv6Address(const IPv4Address &addr) {
  std::memcpy(&addr_[12], &addr, 4);
  addr_[10] = 0xFF;
  addr_[11] = 0xFF;
  std::memset(addr_.data(), 0, 10);
}

IPv6Address::IPv6Address(const ArrayType &a) : addr_(a) {
}

IPv6Address::IPv6Address(const std::string &s) {
  if (!parse(s))
    addr_.fill(0);
}

UInt32 IPv6Address::zone() const {
  if (!isLinkLocal())
    return 0;
  const Big16 *lo = Big16_cast(&addr_[2]);
  const Big16 *hi = Big16_cast(&addr_[4]);
  UInt32 x = *hi;
  return (x << 16) | *lo;
}

void IPv6Address::setZone(UInt32 zone) {
  assert(isLinkLocal());

  *Big16_cast(&addr_[2]) = UInt16_narrow_cast(zone);
  *Big16_cast(&addr_[4]) = UInt16_narrow_cast(zone >> 16);
}

bool IPv6Address::parse(const std::string &s) {
  // Check for %zone option.
  auto pct = s.find('%');
  if (pct != s.npos) {
    // If it's not an IPv6 address, the parse fails.
    if (!parseIPv6Address(s.substr(0, pct)))
      return false;

    // If IPv6 address is not link-local, the parse fails.
    if (!isLinkLocal())
      return false;

    // If zone is not an integer, the parse fails.
    auto zoneStr = s.substr(pct + 1);
    if (zoneStr.empty())
      return false;

    char *end = nullptr;
    UInt64 zone = std::strtoul(zoneStr.c_str(), &end, 10);
    if (*end != '\0' || zone > UINT32_MAX)
      return false;

    setZone(UInt32_narrow_cast(zone));

    return true;
  }

  assert(pct == std::string::npos);

  if (parseIPv6Address(s))
    return true;

  return parseIPv4Address(s);
}

bool IPv6Address::parseIPv6Address(const std::string &s) {
  int result = inet_pton(AF_INET6, s.c_str(), addr_.data());
  return result > 0;
#if 0
  std::error_code err;
  auto addr6 = asio::ip::address_v6::from_string(s, err);
  if (!err) {
    addr_ = addr6.to_bytes();
    return true;
  }
  return false;
#endif //0
}

bool IPv6Address::parseIPv4Address(const std::string &s) {
  IPv4Address addr;
  if (addr.parse(s)) {
    *this = IPv6Address(addr);
    return true;
  }
  return false;
}

void IPv6Address::clear() {
  std::memset(addr_.data(), 0, sizeof(addr_));
}

std::string IPv6Address::toString() const {
  if (isV4Mapped()) {
    IPv4Address v4 = toV4();
    return v4.toString();
  }

  if (isLinkLocal()) {
    // Check for embedded IPv6 zone in link-local address.
    UInt32 z = zone();
    if (z) {
      IPv6Address addr{*this};
      addr.setZone(0);
      return addr.toString() + '%' + std::to_string(z);
    }
  }

  char ipv6str[INET6_ADDRSTRLEN] = {};
  const char *result =
      inet_ntop(AF_INET6, addr_.data(), ipv6str, sizeof(ipv6str));
  return result ? ipv6str : "<inet_ntop_error6>";
}
