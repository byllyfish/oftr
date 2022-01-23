// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DATAPATHID_H_
#define OFP_DATAPATHID_H_

#include <array>

#include "ofp/macaddress.h"

namespace ofp {

class DatapathID {
 public:
  enum { Length = 8 };

  using ArrayType = std::array<UInt8, Length>;

  DatapathID() : dpid_{} {}
  explicit DatapathID(const ArrayType dpid) { dpid_ = dpid; }
  explicit DatapathID(UInt16 implementerDefined, MacAddress macAddress);
  explicit DatapathID(llvm::StringRef dpid);

  bool empty() const { return toUInt64() == 0; }
  UInt16 implementerDefined() const;
  MacAddress macAddress() const;
  std::string toString() const { return detail::ToString(*this); }
  const ArrayType &toArray() const { return dpid_; }

  bool parse(llvm::StringRef s);
  void clear() { dpid_.fill(0); }

  bool operator==(const DatapathID &rhs) const { return dpid_ == rhs.dpid_; }
  bool operator!=(const DatapathID &rhs) const { return dpid_ != rhs.dpid_; }
  bool operator<(const DatapathID &rhs) const { return dpid_ < rhs.dpid_; }
  bool operator>(const DatapathID &rhs) const { return dpid_ > rhs.dpid_; }
  bool operator<=(const DatapathID &rhs) const { return dpid_ <= rhs.dpid_; }
  bool operator>=(const DatapathID &rhs) const { return dpid_ >= rhs.dpid_; }

 private:
  OFP_ALIGNAS(8) ArrayType dpid_;

  UInt64 toUInt64() const { return *Interpret_cast<UInt64>(&dpid_); }

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                       const DatapathID &value);
};

static_assert(sizeof(DatapathID) == 8, "Unexpected size.");
static_assert(alignof(DatapathID) == 8, "Unexpected alignment.");
static_assert(IsStandardLayout<DatapathID>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<DatapathID>(),
              "Expected trivially copyable.");

}  // namespace ofp

namespace std {

template <>
struct hash<ofp::DatapathID> {
  size_t operator()(const ofp::DatapathID &addr) const {
    return ofp::hash::MurmurHash32(&addr);
  }
};

}  // namespace std

#endif  // OFP_DATAPATHID_H_
