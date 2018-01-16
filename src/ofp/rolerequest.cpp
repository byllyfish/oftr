// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rolerequest.h"
#include "ofp/writable.h"

using namespace ofp;

RoleRequestBuilder::RoleRequestBuilder(const RoleRequest *msg) : msg_{*msg} {}

UInt32 RoleRequestBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_);

  msg_.header_.setLength(msgLen);
  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return xid;
}
