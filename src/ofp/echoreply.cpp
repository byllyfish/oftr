// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/echoreply.h"
#include "ofp/message.h"
#include "ofp/writable.h"
#include "ofp/echorequest.h"

using namespace ofp;

bool EchoReply::isKeepAlive() const {
  return header_.xid() == EchoRequest::kKeepAliveXID && echoData() == EchoRequest::kKeepAliveData;
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
