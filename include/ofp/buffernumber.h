// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_BUFFERNUMBER_H_
#define OFP_BUFFERNUMBER_H_

namespace ofp {

enum OFPBufferNo : UInt32;

class BufferNumber {
 public:
  constexpr BufferNumber() = default;
  constexpr BufferNumber(UInt32 id) : buffer_{id} {}

  // This is a convenience constructor (for efficiency).
  constexpr BufferNumber(Big32 id) : buffer_{id} {}

  constexpr operator OFPBufferNo() const {
    return static_cast<OFPBufferNo>(value());
  }

  bool operator==(const BufferNumber &rhs) const { return buffer_ == rhs.buffer_; }
  bool operator!=(const BufferNumber &rhs) const { return !(*this == rhs); }

 private:
  Big32 buffer_;

  constexpr UInt32 value() const { return buffer_; }
};

}  // namespace ofp

#endif  // OFP_BUFFERNUMBER_H_
