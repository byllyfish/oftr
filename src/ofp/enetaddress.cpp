// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/enetaddress.h"

using namespace ofp;

EnetAddress::EnetAddress(const std::string &s) {
  if (!parse(s)) {
    clear();
  }
}

bool EnetAddress::parse(const std::string &s) {
  return HexToRawData(s, addr_.data(), sizeof(addr_)) >= sizeof(addr_);
}

std::string EnetAddress::toString() const {
  // Output is lower-case hexadecimal and delimited by ':'.
  const char hex[] = "0123456789abcdef";
  const UInt8 *e = addr_.data();
  char buf[17];

  char *p = buf;
  *p++ = hex[*e >> 4];
  *p++ = hex[*e++ & 0x0F];
  for (int i = 0; i < 5; ++i) {
    *p++ = ':';
    *p++ = hex[*e >> 4];
    *p++ = hex[*e++ & 0x0F];
  }

  return std::string(buf, 17);
}
