// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/featuresreply.h"
#include "ofp/channel.h"
#include "ofp/log.h"

using namespace ofp;

#if 0
static OFPActionTypeFlags MapToV1ActionTypeFlags(UInt32 actions) {
  UInt32 result = actions & 0x0007;
  UInt32 moved = (actions & 0x07F8) << 1;
  result |= moved;
  if (actions & OFPATF_STRIP_VLAN_V1)
    result |= 1 << deprecated::v1::OFPAT_STRIP_VLAN;
  if (actions & OFPATF_ENQUEUE_V1)
    result |= 1 << deprecated::v1::OFPAT_ENQUEUE;
  return static_cast<OFPActionTypeFlags>(result);
}

static OFPActionTypeFlags MapFromV1ActionTypeFlags(UInt32 actions) {
  UInt32 result = actions & 0x0007;
  UInt32 moved = (actions & 0x0FF0) >> 1;
  result |= moved;
  if (actions & (1 << deprecated::v1::OFPAT_STRIP_VLAN))
    result |= OFPATF_STRIP_VLAN_V1;
  if (actions & (1 << deprecated::v1::OFPAT_ENQUEUE)) 
    result |= OFPATF_ENQUEUE_V1;
  return static_cast<OFPActionTypeFlags>(result);
}
#endif //0

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

  // Make sure the size of the list is a multiple of the port size. (Note
  // that we mean post-1.0 port size; validation happens after transmogrify.)
  size_t portListSize = header_.length() - sizeof(FeaturesReply);

  return (portListSize % sizeof(Port)) == 0;
}

PortRange FeaturesReply::ports() const {
  assert(header_.length() >= sizeof(FeaturesReply));

  return ByteRange{BytePtr(this) + sizeof(FeaturesReply),
                   header_.length() - sizeof(FeaturesReply)};
}

FeaturesReplyBuilder::FeaturesReplyBuilder(const FeaturesReply *msg)
    : msg_{*msg} {}

FeaturesReplyBuilder::FeaturesReplyBuilder(UInt32 xid) {
  // Set xid of reply to request's xid.
  msg_.header_.setXid(xid);
}

void FeaturesReplyBuilder::setActions(OFPActionTypeFlags actions) {
  msg_.reserved_ = actions;
}

UInt32 FeaturesReplyBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  PortRange portRange{ports_};

  size_t msgLen = sizeof(msg_);
  if (version < OFP_VERSION_4) {
    msgLen += portRange.writeSize(channel);
    msg_.auxiliaryId_ = 0;
  }

  auto savedReserved = msg_.reserved_;

  if (version == OFP_VERSION_1) {
    msg_.reserved_ = OFPActionTypeFlagsConvertToV1(msg_.reserved_);
  } else {
    msg_.reserved_ = 0;
  }

  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  channel->write(&msg_, sizeof(msg_));

  if (version < OFP_VERSION_4) {
    portRange.write(channel);
  }

  channel->flush();

  msg_.reserved_ = savedReserved;

  return msg_.header_.xid();
}
