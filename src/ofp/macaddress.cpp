// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/macaddress.h"

using namespace ofp;

MacAddress::MacAddress(const std::string &s) {
  if (!parse(s)) {
    clear();
  }
}

bool MacAddress::parse(const std::string &s) {
  return HexToRawData(s, addr_.data(), addr_.size()) >= addr_.size();
}

std::string MacAddress::toString() const {
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
