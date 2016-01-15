// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PORTNUMBER_H_
#define OFP_PORTNUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPPortNo : UInt32;

class PortNumber {
 public:
  constexpr PortNumber(UInt32 port = 0) : port_{port} {}

  // This is a convenience constructor (for efficiency).
  constexpr PortNumber(Big32 port) : port_{port} {}

  constexpr operator OFPPortNo() const {
    return static_cast<OFPPortNo>(value());
  }

  bool operator==(const PortNumber &rhs) const { return port_ == rhs.port_; }
  bool operator!=(const PortNumber &rhs) const { return !(*this == rhs); }

  // Handle conversions between 16-bit and 32-bit port numbers. Sign-extend the
  // to 32-bits the "fake" ports.
  constexpr UInt16 toV1() const { return UInt16_narrow_cast(port_); }

  constexpr static PortNumber fromV1(UInt16 port) {
    return port > 0xFF00U ? port | 0xFFFF0000UL : port;
  }

 private:
  Big32 port_;

  constexpr UInt32 value() const { return port_; }
};

static_assert(sizeof(PortNumber) == 4, "Unexpected size.");

}  // namespace ofp

#endif  // OFP_PORTNUMBER_H_
