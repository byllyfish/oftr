// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/groupmod.h"
#include "ofp/writable.h"

using namespace ofp;

BucketRange GroupMod::buckets() const {
  size_t len = header_.length();
  assert(len >= sizeof(GroupMod));

  return BucketRange{
      ByteRange{BytePtr(this) + sizeof(GroupMod), len - sizeof(GroupMod)}};
}

bool GroupMod::validateInput(Validation *context) const {
  if (!buckets().validateInput(context)) {
    return false;
  }
  return true;
}

GroupModBuilder::GroupModBuilder(const GroupMod *msg)
    : msg_{*msg}, buckets_{msg->buckets()} {}

UInt32 GroupModBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  size_t msgLen = sizeof(msg_) + buckets_.size();

  msg_.header_.setVersion(channel->version());
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->write(buckets_.data(), buckets_.size());
  channel->flush();

  return xid;
}
