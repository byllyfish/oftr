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
  UInt32 masterPacketInMask() const { return packetInMask_[0]; }
  UInt32 slavePacketInMask() const { return packetInMask_[1]; }
  UInt32 masterPortStatusMask() const { return portStatusMask_[0]; }
  UInt32 slavePortStatusMask() const { return portStatusMask_[1]; }
  UInt32 masterFlowRemovedMask() const { return flowRemovedMask_[0]; }
  UInt32 slaveFlowRemovedMask() const { return flowRemovedMask_[1]; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big32 packetInMask_[2];
  Big32 portStatusMask_[2];
  Big32 flowRemovedMask_[2];

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

  void setMasterPacketInMask(UInt32 mask) { msg_.packetInMask_[0] = mask; }
  void setSlavePacketInMask(UInt32 mask) { msg_.packetInMask_[1] = mask; }
  void setMasterPortStatusMask(UInt32 mask) { msg_.portStatusMask_[0] = mask; }
  void setSlavePortStatusMask(UInt32 mask) { msg_.portStatusMask_[1] = mask; }
  void setMasterFlowRemovedMask(UInt32 mask) {
    msg_.flowRemovedMask_[0] = mask;
  }
  void setSlaveFlowRemovedMask(UInt32 mask) { msg_.flowRemovedMask_[1] = mask; }

  UInt32 send(Writable *channel);

 private:
  GetAsyncReply msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_GETASYNCREPLY_H_
