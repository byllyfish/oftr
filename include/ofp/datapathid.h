// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_DATAPATHID_H_
#define OFP_DATAPATHID_H_

#include "ofp/types.h"
#include "ofp/array.h"
#include "ofp/macaddress.h"
#include "ofp/byteorder.h"

namespace ofp {

class DatapathID {
 public:
  enum { Length = 8 };

  using ArrayType = std::array<UInt8, Length>;

  DatapathID() : dpid_{} {}
  explicit DatapathID(const ArrayType dpid) { dpid_ = dpid; }
  explicit DatapathID(Big16 implementerDefined, MacAddress macAddress);
  explicit DatapathID(const std::string &dpid);

  bool empty() const { return toUInt64() == 0; }
  Big16 implementerDefined() const;
  MacAddress macAddress() const;
  std::string toString() const;

  bool parse(const std::string &s);
  void clear() { dpid_.fill(0); }

  bool operator<(const DatapathID &rhs) const { return dpid_ < rhs.dpid_; }
  bool operator>(const DatapathID &rhs) const { return dpid_ > rhs.dpid_; }
  bool operator==(const DatapathID &rhs) const { return dpid_ == rhs.dpid_; }
  bool operator!=(const DatapathID &rhs) const { return !operator==(rhs); }

 private:
  ArrayType dpid_;

  UInt64 toUInt64() const { return *Interpret_cast<UInt64>(&dpid_); }
};

std::ostream &operator<<(std::ostream &os, const DatapathID &value);

inline std::ostream &operator<<(std::ostream &os, const DatapathID &value) {
  return os << value.toString();
}

}  // namespace ofp

#endif  // OFP_DATAPATHID_H_
