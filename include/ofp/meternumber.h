// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_METERNUMBER_H_
#define OFP_METERNUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPMeterNo : UInt32;

class MeterNumber {
 public:
  constexpr MeterNumber(UInt32 meter = 0) : meter_{meter} {}

  // This is a convenience constructor (for efficiency).
  constexpr MeterNumber(Big32 meter) : meter_{meter} {}

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
