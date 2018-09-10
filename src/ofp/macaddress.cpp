// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/macaddress.h"
#include "ofp/byteorder.h"

using namespace ofp;

MacAddress::MacAddress(llvm::StringRef s) {
  if (!parse(s)) {
    clear();
  }
}

bool MacAddress::parse(llvm::StringRef s) {
  if (HexDelimitedToRawData(s, addr_.data(), addr_.size()) == addr_.size()) {
    return true;
  }

  // If string is exactly 12 chars, check if we can parse it as hex.
  if (s.size() == 12 && HexStrictToRawData(s, addr_.data(), addr_.size()) == addr_.size()) {
    return true;
  }

  // Handle MAC address with hyphen for delimiter.
  return HexDelimitedToRawData(s, addr_.data(), addr_.size(), '-') == addr_.size();
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const MacAddress &value) {
  char buf[sizeof(MacAddress) * 3];
  return os << RawDataToHexDelimitedLowercase(value.addr_, buf);
}

}  // namespace ofp
