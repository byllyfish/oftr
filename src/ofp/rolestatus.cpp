// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rolestatus.h"
#include "ofp/writable.h"
#include "ofp/rolestatusproperty.h"

using namespace ofp;

PropertyRange RoleStatus::properties() const {
  return SafeByteRange(this, header_.length(), sizeof(RoleStatus));
}

bool RoleStatus::validateInput(Validation *context) const {
  PropertyRange props = properties();
  if (!props.validateInput(context)) {
    return false;
  }

  if (!RoleStatusPropertyValidator::validateInput(props, context)) {
    return false;
  }

  return true;
}

void RoleStatusBuilder::setProperties(const PropertyList &properties) {
  properties_ = properties;
}

UInt32 RoleStatusBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(msg_) + properties_.size();

  msg_.header_.setLength(msgLen);
  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->write(properties_.data(), properties_.size());
  channel->flush();

  return xid;
}
