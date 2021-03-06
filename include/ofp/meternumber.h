// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_METERNUMBER_H_
#define OFP_METERNUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPMeterNo : UInt32;

class MeterNumber {
 public:
  /* implicit NOLINT */ constexpr MeterNumber(UInt32 meter = 0)
      : meter_{meter} {}

  // This is a convenience constructor (for efficiency).
  /* implicit NOLINT */ constexpr MeterNumber(Big32 meter) : meter_{meter} {}

  constexpr operator OFPMeterNo() const {
    return static_cast<OFPMeterNo>(value());
  }

  bool operator==(const MeterNumber &rhs) const { return meter_ == rhs.meter_; }
  bool operator!=(const MeterNumber &rhs) const { return !(*this == rhs); }

 private:
  Big32 meter_;

  constexpr UInt32 value() const { return meter_; }
};

static_assert(sizeof(MeterNumber) == 4, "Unexpected size.");

}  // namespace ofp

#endif  // OFP_METERNUMBER_H_
