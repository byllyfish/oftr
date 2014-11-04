// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/getasyncreply.h"

using namespace ofp;

GetAsyncReplyBuilder::GetAsyncReplyBuilder(UInt32 xid) {
  msg_.header_.setXid(xid);
}

GetAsyncReplyBuilder::GetAsyncReplyBuilder(const GetAsyncRequest *request) {
  msg_.header_.setXid(request->xid());
}

GetAsyncReplyBuilder::GetAsyncReplyBuilder(const GetAsyncReply *msg)
    : msg_{*msg} {}

UInt32 GetAsyncReplyBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_);

  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return msg_.header_.xid();
}
