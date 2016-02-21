// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/featuresreply.h"
#include "ofp/channel.h"
#include "ofp/log.h"

using namespace ofp;

OFPCapabilitiesFlags FeaturesReply::capabilities() const {
  // Version 1.0 uses OFPC_STP in place of OFPC_GROUP_STATS.
  if (header_.version() == OFP_VERSION_1) {
    return OFPCapabilitiesFlagsConvertFromV1(capabilities_);
  }
  return capabilities_;
}

OFPActionTypeFlags FeaturesReply::actions() const {
  if (header_.version() == OFP_VERSION_1) {
    return OFPActionTypeFlagsConvertFromV1(reserved_);
  }
  return static_cast<OFPActionTypeFlags>(static_cast<UInt32>(reserved_));
}

bool FeaturesReply::validateInput(Validation *context) const {
  UInt8 version = header_.version();

  // Version 4 and later don't include the port list.
  if (version >= OFP_VERSION_4) {
    return (context->length() == sizeof(FeaturesReply));
  }

  context->setLengthRemaining(header_.length() - sizeof(FeaturesReply));
  return ports().validateInput(context);
}

PortRange FeaturesReply::ports() const {
  assert(header_.length() >= sizeof(FeaturesReply));

  return ByteRange{BytePtr(this) + sizeof(FeaturesReply),
                   header_.length() - sizeof(FeaturesReply)};
}

FeaturesReplyBuilder::FeaturesReplyBuilder(const FeaturesReply *msg)
    : msg_{*msg} {
}

FeaturesReplyBuilder::FeaturesReplyBuilder(UInt32 xid) {
  // Set xid of reply to request's xid.
  msg_.header_.setXid(xid);
}

UInt32 FeaturesReplyBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  PortRange portRange{ports_};

  size_t msgLen = sizeof(msg_);
  if (version < OFP_VERSION_4) {
    msgLen += portRange.writeSize(channel);
    msg_.auxiliaryId_ = 0;
  }

  auto savedCapabilities = msg_.capabilities_;
  auto savedReserved = msg_.reserved_;

  if (version == OFP_VERSION_1) {
    msg_.capabilities_ = OFPCapabilitiesFlagsConvertToV1(msg_.capabilities_);
    msg_.reserved_ = OFPActionTypeFlagsConvertToV1(msg_.reserved_);
  } else {
    msg_.reserved_ = 0;
  }

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);

  channel->write(&msg_, sizeof(msg_));

  if (version < OFP_VERSION_4) {
    portRange.write(channel);
  }

  channel->flush();

  msg_.capabilities_ = savedCapabilities;
  msg_.reserved_ = savedReserved;

  return msg_.header_.xid();
}
