// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_QUEUEDESCPROPERTY_H_
#define OFP_QUEUEDESCPROPERTY_H_

#include "ofp/experimenterproperty.h"
#include "ofp/constants.h"
#include "ofp/padding.h"

namespace ofp {

class QueueDescPropertyMinRate : private NonCopyable {
 public:
  constexpr static OFPQueueDescProperty type() { return OFPQDPT_MIN_RATE; }

  using ValueType = UInt16;

  explicit QueueDescPropertyMinRate(UInt16 rate) : rate_{rate} {}

  UInt16 value() const { return rate_; }
  static UInt16 defaultValue() { return 0xffff; }

 private:
  Big16 type_ = type();
  Big16 len_ = 8;
  Big16 rate_;
  Padding<2> pad_;
};

static_assert(sizeof(QueueDescPropertyMinRate) == 8, "Unexpected size.");

class QueueDescPropertyMaxRate : private NonCopyable {
 public:
  constexpr static OFPQueueDescProperty type() { return OFPQDPT_MAX_RATE; }

  using ValueType = UInt16;

  explicit QueueDescPropertyMaxRate(UInt16 rate) : rate_{rate} {}

  UInt16 value() const { return rate_; }
  static UInt16 defaultValue() { return 0xffff; }

 private:
  Big16 type_ = type();
  Big16 len_ = 8;
  Big16 rate_;
  Padding<2> pad_;
};

static_assert(sizeof(QueueDescPropertyMaxRate) == 8, "Unexpected size.");

using QueueDescPropertyExperimenter =
    detail::ExperimenterProperty<OFPQueueDescProperty, OFPQDPT_EXPERIMENTER>;

}  // namespace ofp

#endif  // OFP_QUEUEDESCPROPERTY_H_