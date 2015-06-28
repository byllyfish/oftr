// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/queue.h"

using namespace ofp;

PropertyRange Queue::properties() const {
  return SafeByteRange(this, len_, sizeof(Queue));
}

bool Queue::validateInput(Validation *context) const {
  if (len_ < sizeof(Queue))
    return false;

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
