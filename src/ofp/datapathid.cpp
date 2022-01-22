// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/datapathid.h"

#include "ofp/byteorder.h"

using namespace ofp;

DatapathID::DatapathID(UInt16 implementerDefined, MacAddress macAddress) {
  Big16 impDef = implementerDefined;
  std::memcpy(dpid_.data(), &impDef, sizeof(impDef));
  std::memcpy(dpid_.data() + 2, &macAddress, sizeof(macAddress));
}

DatapathID::DatapathID(llvm::StringRef dpid) {
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
  // Check for colon delimited hex format: "hh:hh:hh:hh:hh:hh:hh:hh"
  if (HexDelimitedToRawData(s, dpid_.data(), dpid_.size()) == dpid_.size()) {
    return true;
  }

  // Check if string is a decimal or hexadecimal number (0x).
  UInt64 n;
  if (!s.getAsInteger<UInt64>(0, n)) {
    Big64 val = n;
    std::memcpy(&dpid_, &val, sizeof(dpid_));
    return true;
  }

  return false;
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const DatapathID &value) {
  char buf[sizeof(DatapathID) * 3];
  return os << RawDataToHexDelimitedLowercase(value.dpid_, buf);
}

}  // namespace ofp
