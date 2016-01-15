// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mptablestats.h"
#include "ofp/writable.h"
#include "ofp/constants.h"
#include "ofp/log.h"
#include "ofp/validation.h"

using namespace ofp;

void MPTableStatsBuilder::write(Writable *channel) {
  UInt8 version = channel->version();

  if (version >= OFP_VERSION_4) {
    channel->write(&msg_, 4);
    channel->write(&msg_.activeCount_, 20);
    channel->flush();

  } else if (version == OFP_VERSION_1) {
    channel->write(&msg_, sizeof(msg_));
    channel->flush();

  } else {
    log::debug("MPTableStatsBuilder not implemented for version:", version);
  }
}
