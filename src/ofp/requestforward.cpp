// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/requestforward.h"
#include "ofp/writable.h"

using namespace ofp;

ByteRange RequestForward::request() const {
  assert(header_.length() >= sizeof(RequestForward));
  return ByteRange{BytePtr(this) + sizeof(RequestForward),
                   header_.length() - sizeof(RequestForward)};
}

bool RequestForward::validateInput(Validation *context) const {
  const ByteRange req = request();
  const Header *reqHeader = reinterpret_cast<const Header *>(req.data());

  if (reqHeader->length() != req.size()) {
    log::info("RequestForward inner request size is invalid");
    return false;
  }

  return true;
}

UInt32 RequestForwardBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_) + request_.size();

  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->write(request_.data(), request_.size());
  channel->flush();

  return xid;
}
