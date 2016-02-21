// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/echoreply.h"
#include "ofp/message.h"
#include "ofp/writable.h"

using namespace ofp;

ByteRange EchoReply::echoData() const {
  return ByteRange{BytePtr(this) + sizeof(Header),
                   header_.length() - sizeof(Header)};
}

EchoReplyBuilder::EchoReplyBuilder(UInt32 xid) {
  msg_.header_.setXid(xid);
}

void EchoReplyBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_) + data_.size();

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);

  channel->write(&msg_, sizeof(msg_));
  channel->write(data_.data(), data_.size());
  channel->flush();
}
