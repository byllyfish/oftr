// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/portmod.h"
#include "ofp/writable.h"

using namespace ofp;

PortModBuilder::PortModBuilder(const PortMod *msg) : msg_{*msg} {}

UInt32 PortModBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();
  size_t msgLen = sizeof(msg_);

  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);

  if (version == OFP_VERSION_1) {
    // Temporarily set the message type to the V1 value.
    msg_.header_.setType(deprecated::v1::OFPT_PORT_MOD);
    msg_.header_.setLength(UInt16_narrow_cast(msgLen - 8));
    channel->write(&msg_.header_, sizeof(Header));
    msg_.header_.setType(OFPT_PORT_MOD);

    channel->write(BytePtr(&msg_.portNo_) + 2, 2);
    channel->write(&msg_.hwAddr_, 6);
    channel->write(&msg_.config_, 16);

  } else {
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));
    channel->write(&msg_, sizeof(msg_));
  }
  channel->flush();

  return xid;
}
