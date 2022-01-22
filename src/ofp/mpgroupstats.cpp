// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpgroupstats.h"

#include "ofp/validation.h"
#include "ofp/writable.h"

using namespace ofp;

PacketCounterRange MPGroupStats::bucketStats() const {
  return SafeByteRange(this, length_, sizeof(MPGroupStats));
}

bool MPGroupStats::validateInput(Validation *context) const {
  if (!context->validateAlignedLength(length_, sizeof(MPGroupStats))) {
    return false;
  }

  return bucketStats().validateInput(context);
}

void MPGroupStatsBuilder::write(Writable *channel) {
  UInt8 version = channel->version();
  size_t partLen = (version > OFP_VERSION_3) ? sizeof(msg_) : sizeof(msg_) - 8;

  msg_.length_ = UInt16_narrow_cast(partLen + bucketStats_.size());

  channel->write(&msg_, partLen);
  channel->write(bucketStats_.data(), bucketStats_.size());
  channel->flush();
}
