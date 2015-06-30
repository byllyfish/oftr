// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/queue.h"
#include "ofp/validation.h"

using namespace ofp;

PropertyRange Queue::properties() const {
  // N.B. This length_ is padded out; just in case.
  return SafeByteRange(this, PadLength(len_), sizeof(Queue));
}

bool Queue::validateInput(Validation *context) const {
  if (len_ < sizeof(Queue)) {
    log::info("Length of queue is too small", len_);
    return false;
  }

  PropertyRange props = properties();
  if (!props.validateInput(context))
    return false;

  if (!QueuePropertyValidator::validateInput(props, context))
    return false;

  return true;
}

void QueueBuilder::setProperties(const PropertyRange &properties) {
  properties_.assign(properties);
  queue_.len_ = UInt16_narrow_cast(SizeWithoutProperties + properties_.size());
}
