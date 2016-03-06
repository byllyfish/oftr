// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/setconfig.h"
#include "ofp/writable.h"

using namespace ofp;

SetConfigBuilder::SetConfigBuilder(const SetConfig *msg) : msg_{*msg} {}

UInt32 SetConfigBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();

  msg_.header_.setVersion(channel->version());
  msg_.header_.setLength(sizeof(msg_));
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return xid;
}
