// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/error.h"
#include "ofp/message.h"
#include "ofp/writable.h"
#include "ofp/log.h"

using namespace ofp;

ByteRange Error::errorData() const {
  return ByteRange{BytePtr(this) + sizeof(Error),
                   header_.length() - sizeof(Error)};
}

ErrorBuilder::ErrorBuilder(UInt32 xid) { msg_.header_.setXid(xid); }

ErrorBuilder::ErrorBuilder(const Error *msg) : msg_{*msg} {
  // Only data is left to copy.
  ByteRange data = msg->errorData();
  setErrorData(data.data(), data.size());
}

void ErrorBuilder::setErrorData(const Message *message) {
  assert(message);
  size_t len = message->size();
  if (len > 64) len = 64;
  setErrorData(message->data(), len);
}

void ErrorBuilder::send(Writable *channel) {
  size_t msgLen = sizeof(msg_) + data_.size();

  // Send v1 error message when channel version is unknown.
  UInt8 version = channel->version();
  if (version == 0) {
    version = OFP_VERSION_1;
  }

  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setXid(channel->nextXid());

  channel->write(&msg_, sizeof(msg_));
  channel->write(data_.data(), data_.size());
  channel->flush();
}
