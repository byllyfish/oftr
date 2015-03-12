// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/setasync.h"
#include "ofp/writable.h"

using namespace ofp;

SetAsyncBuilder::SetAsyncBuilder(const SetAsync *msg) : msg_{*msg} {
}

UInt32 SetAsyncBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_);

  msg_.header_.setXid(xid);
  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return xid;
}
