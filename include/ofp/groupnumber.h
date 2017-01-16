// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_GROUPNUMBER_H_
#define OFP_GROUPNUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPGroupNo : UInt32;

class GroupNumber {
 public:
  constexpr GroupNumber(UInt32 group = 0) : group_{group} {}

  // This is a convenience constructor (for efficiency).
  constexpr GroupNumber(Big32 group) : group_{group} {}

  constexpr operator OFPGroupNo() const {
    return static_cast<OFPGroupNo>(value());
  }

  bool operator==(const GroupNumber &rhs) const { return group_ == rhs.group_; }
  bool operator==(OFPGroupNo rhs) const {
    return group_ == static_cast<UInt32>(rhs);
  }
  bool operator!=(const GroupNumber &rhs) const { return !(*this == rhs); }

 private:
  Big32 group_;

  constexpr UInt32 value() const { return group_; }
};

}  // namespace ofp

#endif  // OFP_GROUPNUMBER_H_
