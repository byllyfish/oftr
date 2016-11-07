// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/queue.h"
#include "ofp/validation.h"
#include "ofp/writable.h"

using namespace ofp;

PropertyRange Queue::properties() const {
  // N.B. This length_ is padded out; just in case.
  return SafeByteRange(this, PadLength(len_), sizeof(Queue));
}

bool Queue::validateInput(Validation *context) const {
  if (len_ < sizeof(Queue)) {
    log_info("Length of queue is too small", len_);
    return false;
  }

  PropertyRange props = properties();
  if (!props.validateInput(context))
    return false;

  if (!QueuePropertyValidator::validateInput(props, context))
    return false;

  return true;
}

QueueBuilder::QueueBuilder(const deprecated::QueueV1 &queue) {
  setQueueId(queue.queueId());
  setPort(0);
  setProperties(queue.properties());
}

void QueueBuilder::setProperties(const PropertyRange &properties) {
  properties_.assign(properties);
  queue_.len_ = UInt16_narrow_cast(SizeWithoutProperties + properties_.size());
}

PropertyRange deprecated::QueueV1::properties() const {
  return SafeByteRange(this, len_, sizeof(*this));
}

bool deprecated::QueueV1::validateInput(Validation *context) const {
  return len_ >= sizeof(QueueV1);
}

deprecated::QueueV1Builder::QueueV1Builder(const Queue &queue)
    : properties_{queue.properties()} {
  queue_.queueId_ = queue.queueId();

  if (!properties_.empty()) {
    queue_.len_ = UInt16_narrow_cast(sizeof(QueueV1) + properties_.size());
  } else {
    queue_.len_ = UInt16_narrow_cast(sizeof(QueueV1) + 8);
  }
}

void deprecated::QueueV1Builder::write(Writable *channel) {
  channel->write(&queue_, sizeof(queue_));

  if (!properties_.empty()) {
    channel->write(properties_.data(), properties_.size());
  } else {
    struct {
      Big16 type = OFPQT_NONE;
      Big16 len_ = 8;
      Padding<4> pad;
    } queueProp;
    static_assert(sizeof(queueProp) == 8, "Unexpected size");
    channel->write(&queueProp, sizeof(queueProp));
  }
}
