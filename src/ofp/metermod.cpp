// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/metermod.h"

#include "ofp/writable.h"

using namespace ofp;

bool MeterMod::validateInput(Validation *context) const {
  return meterBands().validateInput(context);
}

MeterBandRange MeterMod::meterBands() const {
  return SafeByteRange(this, header_.length(), sizeof(MeterMod));
}

UInt32 MeterModBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_) + meterBands_.size();

  msg_.header_.setXid(xid);
  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);

  channel->write(&msg_, sizeof(msg_));
  channel->write(meterBands_.data(), meterBands_.size());

  channel->flush();

  return xid;
}
