// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/setasync.h"
#include "ofp/writable.h"
#include "ofp/asyncconfigproperty.h"

using namespace ofp;

PropertyRange SetAsync::properties() const {
  assert(header_.length() >= sizeof(SetAsync));
  return ByteRange{BytePtr(this) + sizeof(SetAsync),
                   header_.length() - sizeof(SetAsync)};
}

bool SetAsync::validateInput(Validation *context) const {
  return properties().validateInput(context);
}

SetAsyncBuilder::SetAsyncBuilder(const SetAsync *msg) : msg_{*msg} {
}

UInt32 SetAsyncBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();

  msg_.header_.setXid(xid);
  msg_.header_.setVersion(version);

  if (version >= OFP_VERSION_5) {
    size_t msgLen = sizeof(msg_) + properties_.size();
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));

    channel->write(&msg_, sizeof(msg_));
    channel->write(properties_.data(), properties_.size());

  } else {
    struct {
      Big<OFPPacketInFlags> packetInMask[2];
      Big<OFPPortStatusFlags> portStatusMask[2];
      Big<OFPFlowRemovedFlags> flowRemovedMask[2];
    } asyncBody;

    PropertyRange props = properties_.toRange();
    asyncBody.packetInMask[0] =
        props.value<AsyncConfigPropertyPacketInMaster>();
    asyncBody.packetInMask[1] = props.value<AsyncConfigPropertyPacketInSlave>();
    asyncBody.portStatusMask[0] =
        props.value<AsyncConfigPropertyPortStatusMaster>();
    asyncBody.portStatusMask[1] =
        props.value<AsyncConfigPropertyPortStatusSlave>();
    asyncBody.flowRemovedMask[0] =
        props.value<AsyncConfigPropertyFlowRemovedMaster>();
    asyncBody.flowRemovedMask[1] =
        props.value<AsyncConfigPropertyFlowRemovedSlave>();

    msg_.header_.setLength(32);
    channel->write(&msg_, sizeof(msg_));
    channel->write(&asyncBody, sizeof(asyncBody));
  }

  channel->flush();

  return xid;
}
