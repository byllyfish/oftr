// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_TABLENUMBER_H_
#define OFP_TABLENUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPTableNo : UInt8;

class TableNumber {
 public:
  constexpr TableNumber() = default;
  constexpr TableNumber(UInt8 table) : table_{table} {}

  // This is a convenience constructor (for efficiency).
  constexpr TableNumber(Big8 table) : table_{table} {}

  constexpr operator OFPTableNo() const {
    return static_cast<OFPTableNo>(value());
  }

  bool operator==(const TableNumber &rhs) const { return table_ == rhs.table_; }
  bool operator!=(const TableNumber &rhs) const { return !(*this == rhs); }

 private:
  Big8 table_;

  constexpr UInt8 value() const { return table_; }
};

static_assert(sizeof(TableNumber) == 1, "Unexpected size.");

}  // namespace ofp

#endif  // OFP_TABLENUMBER_H_
