// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
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

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const MacAddress &value) {
  char buf[sizeof(MacAddress) * 3];
  return os << RawDataToHexDelimitedLowercase(value.addr_, buf);
}

}  // namespace ofp
