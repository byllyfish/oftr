// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/echorequest.h"
#include "ofp/message.h"
#include "ofp/writable.h"

using namespace ofp;

// A Pass-Thru echo request is ignored by OFTR's connection handler. It begins
// with "__OFTR__".

const ByteRange EchoRequest::kPassThruData = ByteRange{"__OFTR__", 8};
const ByteRange EchoRequest::kKeepAliveData = ByteRange{"??OFTR??", 8};

bool EchoRequest::isPassThru() const {
  ByteRange data = SafeByteRange(this, header_.length(), sizeof(Header), 8);
  return data == kPassThruData;
}

void EchoRequestBuilder::setKeepAlive() {
  msg_.header_.setXid(EchoRequest::kKeepAliveXID);
  setEchoData(EchoRequest::kKeepAliveData.data(),
              EchoRequest::kKeepAliveData.size());
}

UInt32 EchoRequestBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  size_t msgLen = sizeof(Header) + data_.size();
  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);

  // Assign xid only if xid is 0.
  if (msg_.header_.xid() == 0) {
    msg_.header_.setXid(channel->nextXid());
  }

  channel->write(&msg_, sizeof(msg_));
  channel->write(data_.data(), data_.size());
  channel->flush();

  return msg_.header_.xid();
}
