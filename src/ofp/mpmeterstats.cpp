// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpmeterstats.h"

#include "ofp/validation.h"
#include "ofp/writable.h"

using namespace ofp;

PacketCounterRange MPMeterStats::bandStats() const {
  return SafeByteRange(this, len_, sizeof(MPMeterStats));
}

bool MPMeterStats::validateInput(Validation *context) const {
  if (!context->validateAlignedLength(len_, sizeof(MPMeterStats))) {
    return false;
  }

  return bandStats().validateInput(context);
}

void MPMeterStatsBuilder::write(Writable *channel) {
  msg_.len_ = UInt16_narrow_cast(sizeof(msg_) + bandStats_.size());
  channel->write(&msg_, sizeof(msg_));
  channel->write(bandStats_.data(), bandStats_.size());
  channel->flush();
}
