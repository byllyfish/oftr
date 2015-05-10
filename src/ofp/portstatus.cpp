// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/portstatus.h"
#include "ofp/message.h"
#include "ofp/writable.h"

using namespace ofp;

bool PortStatus::validateInput(Validation *context) const {
  size_t length = context->length();

  if (length < sizeof(PortStatus) + sizeof(Port)) {
    log::debug("PortStatus too small", length);
    return false;
  }

  size_t remainingLength = length - sizeof(PortStatus);
  context->setLengthRemaining(remainingLength);

  // FIXME: make sure there is only one port?

  if (!port().validateInput(context)) {
    return false;
  }

  return true;
}

UInt32 PortStatusBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();

  UInt16 msgLen = sizeof(msg_) + port_.writeSize(channel);

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  port_.write(channel);
  channel->flush();

  return xid;
}
