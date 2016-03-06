// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/getconfigreply.h"
#include "ofp/writable.h"

using namespace ofp;

GetConfigReplyBuilder::GetConfigReplyBuilder(const GetConfigReply *msg)
    : msg_{*msg} {}

UInt32 GetConfigReplyBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();

  msg_.header_.setVersion(channel->version());
  msg_.header_.setLength(sizeof(msg_));
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return xid;
}
