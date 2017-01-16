// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
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

std::string DatapathID::toString() const {
  if (empty()) {
    return {};
  }
  char buf[sizeof(DatapathID) * 3];
  return RawDataToHexDelimitedLowercase(dpid_, buf);
}

bool DatapathID::parse(const std::string &s) {
  if (s.empty()) {
    clear();
    return true;
  }

  return HexToRawData(s, dpid_.data(), dpid_.size()) >= dpid_.size();
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const DatapathID &value) {
  char buf[sizeof(DatapathID) * 3];
  return os << RawDataToHexDelimitedLowercase(value.dpid_, buf);
}

}  // namespace ofp
