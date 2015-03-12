// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/portstatus.h"
#include "ofp/message.h"
#include "ofp/writable.h"

using namespace ofp;

PortStatusBuilder::PortStatusBuilder(const PortStatus *msg) : msg_{*msg} {
}

UInt32 PortStatusBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();

  UInt16 msgLen = sizeof(msg_) + sizeof(Port);
  if (version == OFP_VERSION_1) {
    // Subtract difference between size of Port and PortV1.
    msgLen -= 16;
  }

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));

  if (version == OFP_VERSION_1) {
    deprecated::PortV1 port{port_.toPort()};
    channel->write(&port, sizeof(port));
  } else {
    port_.write(channel);
  }
  channel->flush();

  return xid;
}
