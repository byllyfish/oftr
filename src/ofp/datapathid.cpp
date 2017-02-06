// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/datapathid.h"
#include "ofp/byteorder.h"

using namespace ofp;

DatapathID::DatapathID(UInt16 implementerDefined, MacAddress macAddress) {
  Big16 impDef = implementerDefined;
  std::memcpy(dpid_.data(), &impDef, sizeof(impDef));
  std::memcpy(dpid_.data() + 2, &macAddress, sizeof(macAddress));
}

DatapathID::DatapathID(const std::string &dpid) {
  if (!parse(dpid))
    clear();
}

UInt16 DatapathID::implementerDefined() const {
  return Big16_unaligned(dpid_.data());
}

MacAddress DatapathID::macAddress() const {
  MacAddress result;
  std::memcpy(&result, dpid_.data() + 2, sizeof(result));
  return result;
}

bool DatapathID::parse(llvm::StringRef s) {
  if (s.empty()) {
    clear();
    return true;
  }

  if (s.startswith("0x")) {
    UInt64 n;
    if (s.getAsInteger<UInt64>(0, n))
      return false;
    Big64 val = n;
    std::memcpy(&dpid_, &val, sizeof(dpid_));
    return true;
  }

  return HexDelimitedToRawData(s, dpid_.data(), dpid_.size()) == dpid_.size();
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const DatapathID &value) {
  char buf[sizeof(DatapathID) * 3];
  return os << RawDataToHexDelimitedLowercase(value.dpid_, buf);
}

}  // namespace ofp
