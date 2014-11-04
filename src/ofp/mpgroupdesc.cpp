// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/mpgroupdesc.h"
#include "ofp/validation.h"

using namespace ofp;

BucketRange MPGroupDesc::buckets() const {
  assert(length_ >= sizeof(MPGroupDesc));

  return ByteRange{BytePtr(this) + sizeof(MPGroupDesc),
                   length_ - sizeof(MPGroupDesc)};
}

bool MPGroupDesc::validateInput(Validation *context) const {
  if (!context->validateLength(length_, sizeof(MPGroupDesc))) {
    return false;
  }

  if (!buckets().validateInput(context)) {
    return false;
  }

  return true;
}

void MPGroupDescBuilder::write(Writable *channel) {
  size_t msgLen = sizeof(msg_) + buckets_.size();

  msg_.length_ = UInt16_narrow_cast(msgLen);

  channel->write(&msg_, sizeof(msg_));
  channel->write(buckets_.data(), buckets_.size());
  channel->flush();
}
