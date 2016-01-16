// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_QUEUENUMBER_H_
#define OFP_QUEUENUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPQueueNo : UInt32;

class QueueNumber {
 public:
  constexpr QueueNumber(UInt32 queue = 0) : queue_{queue} {}

  // This is a convenience constructor (for efficiency).
  constexpr QueueNumber(Big32 queue) : queue_{queue} {}

  constexpr operator OFPQueueNo() const {
    return static_cast<OFPQueueNo>(value());
  }

  bool operator==(const QueueNumber &rhs) const { return queue_ == rhs.queue_; }
  bool operator!=(const QueueNumber &rhs) const { return !(*this == rhs); }

 private:
  Big32 queue_;

  constexpr UInt32 value() const { return queue_; }
};

static_assert(sizeof(QueueNumber) == 4, "Unexpected size.");

}  // namespace ofp

#endif  // OFP_QUEUENUMBER_H_
