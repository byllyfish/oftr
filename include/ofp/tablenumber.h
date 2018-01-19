// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_TABLENUMBER_H_
#define OFP_TABLENUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPTableNo : UInt8;

class TableNumber {
 public:
  constexpr TableNumber() = default;
  /* implicit NOLINT */ constexpr TableNumber(UInt8 table) : table_{table} {}

  // This is a convenience constructor (for efficiency).
  /* implicit NOLINT */ constexpr TableNumber(Big8 table) : table_{table} {}

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
