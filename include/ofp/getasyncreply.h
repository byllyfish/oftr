// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_GETASYNCREPLY_H_
#define OFP_GETASYNCREPLY_H_

#include "ofp/protocolmsg.h"
#include "ofp/headeronly.h"

namespace ofp {

// FIXME - identical to SetAsync? Use template?

class GetAsyncReply
    : public ProtocolMsg<GetAsyncReply, OFPT_GET_ASYNC_REPLY, 32, 32> {
 public:
  OFPPacketInFlags masterPacketInMask() const { return packetInMask_[0]; }
  OFPPacketInFlags slavePacketInMask() const { return packetInMask_[1]; }
  OFPPortStatusFlags masterPortStatusMask() const { return portStatusMask_[0]; }
  OFPPortStatusFlags slavePortStatusMask() const { return portStatusMask_[1]; }
  OFPFlowRemovedFlags masterFlowRemovedMask() const {
    return flowRemovedMask_[0];
  }
  OFPFlowRemovedFlags slaveFlowRemovedMask() const {
    return flowRemovedMask_[1];
  }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big<OFPPacketInFlags> packetInMask_[2];
  Big<OFPPortStatusFlags> portStatusMask_[2];
  Big<OFPFlowRemovedFlags> flowRemovedMask_[2];

  // Only GetAsyncReplyBuilder can construct an instance.
  GetAsyncReply() : header_{type()} {}

  friend class GetAsyncReplyBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(GetAsyncReply) == 32, "Unexpected size.");
static_assert(IsStandardLayout<GetAsyncReply>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<GetAsyncReply>(),
              "Expected trivially copyable.");

class GetAsyncReplyBuilder {
 public:
  explicit GetAsyncReplyBuilder(UInt32 xid);
  explicit GetAsyncReplyBuilder(const GetAsyncRequest *request);
  explicit GetAsyncReplyBuilder(const GetAsyncReply *msg);

  void setMasterPacketInMask(OFPPacketInFlags mask) {
    msg_.packetInMask_[0] = mask;
  }
  void setSlavePacketInMask(OFPPacketInFlags mask) {
    msg_.packetInMask_[1] = mask;
  }
  void setMasterPortStatusMask(OFPPortStatusFlags mask) {
    msg_.portStatusMask_[0] = mask;
  }
  void setSlavePortStatusMask(OFPPortStatusFlags mask) {
    msg_.portStatusMask_[1] = mask;
  }
  void setMasterFlowRemovedMask(OFPFlowRemovedFlags mask) {
    msg_.flowRemovedMask_[0] = mask;
  }
  void setSlaveFlowRemovedMask(OFPFlowRemovedFlags mask) {
    msg_.flowRemovedMask_[1] = mask;
  }

  UInt32 send(Writable *channel);

 private:
  GetAsyncReply msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_GETASYNCREPLY_H_
