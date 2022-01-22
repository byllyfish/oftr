// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/tablestatus.h"

#include "ofp/writable.h"

using namespace ofp;

bool TableStatus::validateInput(Validation *context) const {
  size_t length = context->length();

  if (length < sizeof(TableStatus) + sizeof(TableDesc)) {
    log_debug("TableStatus too small", length);
    return false;
  }

  size_t remainingLength = length - sizeof(TableStatus);
  context->setLengthRemaining(remainingLength);

  // FIXME: make sure there is only one table?
  return table().validateInput(context);
}

UInt32 TableStatusBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();

  UInt16 msgLen = UInt16_narrow_cast(sizeof(msg_) + table_.writeSize(channel));

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  table_.write(channel);
  channel->flush();

  return xid;
}
