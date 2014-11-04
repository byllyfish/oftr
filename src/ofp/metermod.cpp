// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/metermod.h"
#include "ofp/writable.h"

using namespace ofp;

bool MeterMod::validateInput(Validation *context) const {
  if (!meterBands().validateInput(context)) {
    return false;
  }
  return true;
}

MeterBandRange MeterMod::meterBands() const {
  return ByteRange{BytePtr(this) + sizeof(MeterMod),
                   header_.length() - sizeof(MeterMod)};
}

UInt32 MeterModBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_) + meterBands_.size();

  msg_.header_.setXid(xid);
  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  channel->write(&msg_, sizeof(msg_));
  channel->write(meterBands_.data(), meterBands_.size());

  channel->flush();

  return xid;
}
