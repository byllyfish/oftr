// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/groupmod.h"

#include "ofp/writable.h"

using namespace ofp;

BucketRange GroupMod::buckets() const {
  return SafeByteRange(this, header_.length(), sizeof(GroupMod));
}

bool GroupMod::validateInput(Validation *context) const {
  return buckets().validateInput(context);
}

GroupModBuilder::GroupModBuilder(const GroupMod *msg)
    : msg_{*msg}, buckets_{msg->buckets()} {}

UInt32 GroupModBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  size_t msgLen = sizeof(msg_) + buckets_.size();

  msg_.header_.setVersion(channel->version());
  msg_.header_.setLength(msgLen);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->write(buckets_.data(), buckets_.size());
  channel->flush();

  return xid;
}
