// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/echorequest.h"
#include "ofp/message.h"
#include "ofp/writable.h"

using namespace ofp;

ByteRange EchoRequest::echoData() const {
  return ByteRange{BytePtr(this) + sizeof(Header),
                   header_.length() - sizeof(Header)};
}

UInt32 EchoRequestBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  size_t msgLen = sizeof(Header) + data_.size();
  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  // Assign xid only if xid is 0.
  if (msg_.header_.xid() == 0) {
    msg_.header_.setXid(channel->nextXid());
  }

  channel->write(&msg_, sizeof(msg_));
  channel->write(data_.data(), data_.size());
  channel->flush();

  return msg_.header_.xid();
}
