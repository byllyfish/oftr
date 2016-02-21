// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rolereply.h"
#include "ofp/writable.h"

using namespace ofp;

RoleReplyBuilder::RoleReplyBuilder(UInt32 xid) {
  // Set xid of reply to request's xid.
  msg_.header_.setXid(xid);
}

RoleReplyBuilder::RoleReplyBuilder(const RoleRequest *request) {
  msg_.header_.setXid(request->xid());
}

RoleReplyBuilder::RoleReplyBuilder(const RoleReply *msg) : msg_{*msg} {
}

UInt32 RoleReplyBuilder::send(Writable *channel) {
  size_t msgLen = sizeof(msg_);
  UInt8 version = channel->version();

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return msg_.header_.xid();
}
