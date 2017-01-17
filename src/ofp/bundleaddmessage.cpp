// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/bundleaddmessage.h"
#include "ofp/writable.h"

using namespace ofp;

ByteRange BundleAddMessage::message() const {
  assert(header_.length() >= sizeof(BundleAddMessage) + sizeof(Header));
  const Header *msgHeader = reinterpret_cast<const Header *>(
      BytePtr(this) + sizeof(BundleAddMessage));
  assert(header_.length() >= sizeof(BundleAddMessage) + msgHeader->length());
  return ByteRange{msgHeader, msgHeader->length()};
}

PropertyRange BundleAddMessage::properties() const {
  assert(header_.length() >= sizeof(BundleAddMessage) + sizeof(Header));
  const Header *msgHeader = reinterpret_cast<const Header *>(
      BytePtr(this) + sizeof(BundleAddMessage));
  size_t offset = sizeof(BundleAddMessage) + PadLength(msgHeader->length());
  assert(header_.length() >= offset);
  return SafeByteRange(this, header_.length(), offset);
}

bool BundleAddMessage::validateInput(Validation *context) const {
  const Header *msgHeader = reinterpret_cast<const Header *>(
      BytePtr(this) + sizeof(BundleAddMessage));
  if (msgHeader->length() < sizeof(Header)) {
    return false;
  }

  if (header_.length() < sizeof(BundleAddMessage) + msgHeader->length()) {
    return false;
  }

  return true;
}

UInt32 BundleAddMessageBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();

  size_t paddedMessageLen = PadLength(message_.size());
  size_t msgLen = sizeof(msg_) + paddedMessageLen + properties_.size();

  msg_.header_.setLength(msgLen);
  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->write(message_.data(), message_.size(),
                 paddedMessageLen - message_.size());
  channel->write(properties_.data(), properties_.size());
  channel->flush();

  return xid;
}
