// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpgroupdesc.h"
#include "ofp/validation.h"

using namespace ofp;

BucketRange MPGroupDesc::buckets() const {
  assert(length_ >= sizeof(MPGroupDesc));

  return ByteRange{BytePtr(this) + sizeof(MPGroupDesc),
                   length_ - sizeof(MPGroupDesc)};
}

bool MPGroupDesc::validateInput(Validation *context) const {
  if (!context->validateAlignedLength(length_, sizeof(MPGroupDesc))) {
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
