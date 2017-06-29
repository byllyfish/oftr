// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_VLANNUMBER_H_
#define OFP_VLANNUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

class VlanNumber {
 public:
  constexpr VlanNumber() = default;
  constexpr VlanNumber(UInt16 vlan) : vlan_{vlan} {}

  constexpr UInt16 value() const { return vlan_; }

  constexpr operator UInt16() const { return vlan_; }

  bool operator==(const VlanNumber &rhs) const { return vlan_ == rhs.vlan_; }
  bool operator!=(const VlanNumber &rhs) const { return !(*this == rhs); }

  SignedInt32 displayCode() const;
  void setDisplayCode(SignedInt32 code);

 private:
  Big16 vlan_;
};

}  // namespace ofp

#endif  // OFP_VLANNUMBER_H_
