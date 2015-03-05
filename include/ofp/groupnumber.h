// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_GROUPNUMBER_H_
#define OFP_GROUPNUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPGroupNo : UInt32;

class GroupNumber {
 public:
  constexpr GroupNumber() = default;
  constexpr GroupNumber(UInt32 group) : group_{group} {}

  // This is a convenience constructor (for efficiency).
  constexpr GroupNumber(Big32 group) : group_{group} {}

  constexpr operator OFPGroupNo() const {
    return static_cast<OFPGroupNo>(value());
  }

  bool operator==(const GroupNumber &rhs) const { return group_ == rhs.group_; }
  bool operator!=(const GroupNumber &rhs) const { return !(*this == rhs); }

 private:
  Big32 group_;

  constexpr UInt32 value() const { return group_; }
};

}  // namespace ofp

#endif  // OFP_GROUPNUMBER_H_
