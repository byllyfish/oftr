// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/getasyncreply.h"
#include "ofp/asyncconfigproperty.h"

using namespace ofp;

PropertyRange GetAsyncReply::properties() const {
  return SafeByteRange(this, header_.length(), sizeof(GetAsyncReply));
}

bool GetAsyncReply::validateInput(Validation *context) const {
  return properties().validateInput(context);
}

GetAsyncReplyBuilder::GetAsyncReplyBuilder(UInt32 xid) {
  msg_.header_.setXid(xid);
}

GetAsyncReplyBuilder::GetAsyncReplyBuilder(const GetAsyncRequest *request) {
  msg_.header_.setXid(request->xid());
}

GetAsyncReplyBuilder::GetAsyncReplyBuilder(const GetAsyncReply *msg)
    : msg_{*msg} {}

UInt32 GetAsyncReplyBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();

  msg_.header_.setXid(xid);
  msg_.header_.setVersion(version);

  if (version >= OFP_VERSION_5) {
    size_t msgLen = sizeof(msg_) + properties_.size();
    msg_.header_.setLength(msgLen);

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
