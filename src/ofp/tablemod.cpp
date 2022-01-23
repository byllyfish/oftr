// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/tablemod.h"

#include "ofp/tablemodproperty.h"
#include "ofp/writable.h"

using namespace ofp;

PropertyRange TableMod::properties() const {
  return SafeByteRange(this, header_.length(), sizeof(TableMod));
}

bool TableMod::validateInput(Validation *context) const {
  // Versions 1.3 and earlier do not have properties.
  if (header_.version() <= OFP_VERSION_4) {
    return header_.length() == sizeof(TableMod);
  }

  PropertyRange props = properties();
  if (!props.validateInput(context))
    return false;

  return TableModPropertyValidator::validateInput(props, context);
}

UInt32 TableModBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(TableMod);

  if (version >= OFP_VERSION_5) {
    msgLen += properties_.size();
  }

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  if (version >= OFP_VERSION_5) {
    channel->write(properties_.data(), properties_.size());
  }

  channel->flush();

  return xid;
}
