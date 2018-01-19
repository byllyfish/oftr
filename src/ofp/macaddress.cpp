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
  // If string is exactly 12 chars, check if we can parse it as hex.
  if (s.size() == 12) {
    UInt64 n;
    if (s.getAsInteger<UInt64>(16, n))
      return false;
    Big64 val = n;
    std::memcpy(&addr_, BytePtr(&val) + 2, sizeof(addr_));
    return true;
  }

  return HexDelimitedToRawData(s, addr_.data(), addr_.size()) == addr_.size();
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const MacAddress &value) {
  char buf[sizeof(MacAddress) * 3];
  return os << RawDataToHexDelimitedLowercase(value.addr_, buf);
}

}  // namespace ofp
