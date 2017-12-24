// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_BUFFERNUMBER_H_
#define OFP_BUFFERNUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPBufferNo : UInt32;

class BufferNumber {
 public:
  constexpr /* implicit NOLINT */ BufferNumber(UInt32 id = 0) : buffer_{id} {}

  // This is a convenience constructor (for efficiency).
  constexpr /* implicit NOLINT */ BufferNumber(Big32 id) : buffer_{id} {}

  constexpr operator OFPBufferNo() const {
    return static_cast<OFPBufferNo>(value());
  }

  bool operator==(const BufferNumber &rhs) const {
    return buffer_ == rhs.buffer_;
  }

  bool operator==(OFPBufferNo rhs) const {
    return buffer_ == static_cast<UInt32>(rhs);
  }

  bool operator!=(const BufferNumber &rhs) const { return !(*this == rhs); }

 private:
  Big32 buffer_;

  constexpr UInt32 value() const { return buffer_; }
};

}  // namespace ofp

#endif  // OFP_BUFFERNUMBER_H_
