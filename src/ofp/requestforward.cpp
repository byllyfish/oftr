// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/requestforward.h"
#include "ofp/writable.h"

using namespace ofp;

ByteRange RequestForward::request() const {
  return SafeByteRange(this, header_.length(), sizeof(RequestForward));
}

bool RequestForward::validateInput(Validation *context) const {
  const ByteRange req = request();
  const Header *reqHeader = reinterpret_cast<const Header *>(req.data());

  if (reqHeader->length() != req.size()) {
    log_info("RequestForward inner request size is invalid");
    return false;
  }

  return true;
}

UInt32 RequestForwardBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_) + request_.size();

  msg_.header_.setLength(msgLen);
  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->write(request_.data(), request_.size());
  channel->flush();

  return xid;
}
