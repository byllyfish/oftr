// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/bundlecontrol.h"
#include "ofp/writable.h"

using namespace ofp;

PropertyRange BundleControl::properties() const {
  assert(header_.length() >= sizeof(BundleControl));
  return ByteRange{BytePtr(this) + sizeof(BundleControl),
                   header_.length() - sizeof(BundleControl)};
}

bool BundleControl::validateInput(Validation *context) const {
  return true;
}

UInt32 BundleControlBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_) + properties_.size();

  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->write(properties_.data(), properties_.size());
  channel->flush();

  return xid;
}
