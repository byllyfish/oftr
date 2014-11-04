// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/setconfig.h"
#include "ofp/writable.h"

using namespace ofp;

SetConfigBuilder::SetConfigBuilder(const SetConfig *msg) : msg_{*msg} {}

void SetConfigBuilder::setFlags(UInt16 flags) { msg_.flags_ = flags; }

void SetConfigBuilder::setMissSendLen(UInt16 missSendLen) {
  msg_.missSendLen_ = missSendLen;
}

UInt32 SetConfigBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();

  msg_.header_.setVersion(channel->version());
  msg_.header_.setLength(sizeof(msg_));
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return xid;
}
