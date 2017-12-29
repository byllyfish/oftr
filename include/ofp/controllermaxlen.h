// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_CONTROLLERMAXLEN_H_
#define OFP_CONTROLLERMAXLEN_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPControllerMaxLen : UInt16;

class ControllerMaxLen {
 public:
  constexpr ControllerMaxLen() = default;
  /* implicit NOLINT */ constexpr ControllerMaxLen(UInt16 maxlen)
      : maxlen_{maxlen} {}

  // This is a convenience constructor (for efficiency).
  /* implicit NOLINT */ constexpr ControllerMaxLen(Big16 maxlen)
      : maxlen_{maxlen} {}

  constexpr operator OFPControllerMaxLen() const {
    return static_cast<OFPControllerMaxLen>(value());
  }

  bool operator==(const ControllerMaxLen &rhs) const {
    return maxlen_ == rhs.maxlen_;
  }

  bool operator!=(const ControllerMaxLen &rhs) const { return !(*this == rhs); }

  bool operator==(const OFPControllerMaxLen &rhs) const {
    return maxlen_ == rhs;
  }

 private:
  Big16 maxlen_;

  constexpr UInt16 value() const { return maxlen_; }
};

}  // namespace ofp

#endif  // OFP_CONTROLLERMAXLEN_H_
