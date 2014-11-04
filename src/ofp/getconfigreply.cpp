// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/getconfigreply.h"
#include "ofp/writable.h"

using namespace ofp;

GetConfigReplyBuilder::GetConfigReplyBuilder(const GetConfigReply *msg)
    : msg_{*msg} {}

void GetConfigReplyBuilder::setFlags(UInt16 flags) { msg_.flags_ = flags; }

void GetConfigReplyBuilder::setMissSendLen(UInt16 missSendLen) {
  msg_.missSendLen_ = missSendLen;
}

UInt32 GetConfigReplyBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();

  msg_.header_.setVersion(channel->version());
  msg_.header_.setLength(sizeof(msg_));
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return xid;
}
