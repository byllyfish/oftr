// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpmeterconfig.h"
#include "ofp/validation.h"
#include "ofp/writable.h"

using namespace ofp;

MeterBandRange MPMeterConfig::bands() const {
  return SafeByteRange(this, length_, sizeof(MPMeterConfig));
}

bool MPMeterConfig::validateInput(Validation *context) const {
  if (!context->validateAlignedLength(length_, sizeof(MPMeterConfig))) {
    return false;
  }

  return bands().validateInput(context);
}

void MPMeterConfigBuilder::write(Writable *channel) {
  msg_.length_ = UInt16_narrow_cast(sizeof(MPMeterConfig) + bands_.size());

  channel->write(&msg_, sizeof(msg_));
  channel->write(bands_.data(), bands_.size());
  channel->flush();
}
