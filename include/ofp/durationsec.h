// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DURATIONSEC_H_
#define OFP_DURATIONSEC_H_

#include "ofp/byteorder.h"

namespace ofp {

class DurationSec {
 public:
  constexpr DurationSec(UInt32 sec = 0, UInt32 nsec = 0)
      : sec_{sec}, nsec_{nsec} {}

  UInt32 seconds() const { return sec_; }
  UInt32 nanoseconds() const { return nsec_; }

  bool operator==(const DurationSec &rhs) const {
    return sec_ == rhs.sec_ && nsec_ == rhs.nsec_;
  }
  bool operator!=(const DurationSec &rhs) const { return !(*this == rhs); }

  bool parse(llvm::StringRef s);
  std::string toString() const { return detail::ToString(*this); }

 private:
  Big32 sec_;
  Big32 nsec_;

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                       const DurationSec &value);
};

static_assert(sizeof(DurationSec) == 8, "Unexpected size.");
static_assert(alignof(DurationSec) == 4, "Unexpected alignment.");

}  // namespace ofp

#endif  // OFP_DURATIONSEC_H_
