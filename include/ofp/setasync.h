// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_SETASYNC_H_
#define OFP_SETASYNC_H_

#include "ofp/protocolmsg.h"

namespace ofp {

class SetAsync : public ProtocolMsg<SetAsync, OFPT_SET_ASYNC, 32, 32> {
 public:
  OFPPacketInFlags masterPacketInMask() const { return packetInMask_[0]; }
  OFPPacketInFlags slavePacketInMask() const { return packetInMask_[1]; }
  OFPPortStatusFlags masterPortStatusMask() const { return portStatusMask_[0]; }
  OFPPortStatusFlags slavePortStatusMask() const { return portStatusMask_[1]; }
  OFPFlowRemovedFlags masterFlowRemovedMask() const { return flowRemovedMask_[0]; }
  OFPFlowRemovedFlags slaveFlowRemovedMask() const { return flowRemovedMask_[1]; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big<OFPPacketInFlags> packetInMask_[2];
  Big<OFPPortStatusFlags> portStatusMask_[2];
  Big<OFPFlowRemovedFlags> flowRemovedMask_[2];

  // Only SetAsyncBuilder can construct an instance.
  SetAsync() : header_{type()} {}

  friend class SetAsyncBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(SetAsync) == 32, "Unexpected size.");
static_assert(IsStandardLayout<SetAsync>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<SetAsync>(), "Expected trivially copyable.");

class SetAsyncBuilder {
 public:
  SetAsyncBuilder() = default;
  explicit SetAsyncBuilder(const SetAsync *msg);

  void setMasterPacketInMask(OFPPacketInFlags mask) { msg_.packetInMask_[0] = mask; }
  void setSlavePacketInMask(OFPPacketInFlags mask) { msg_.packetInMask_[1] = mask; }
  void setMasterPortStatusMask(OFPPortStatusFlags mask) { msg_.portStatusMask_[0] = mask; }
  void setSlavePortStatusMask(OFPPortStatusFlags mask) { msg_.portStatusMask_[1] = mask; }
  void setMasterFlowRemovedMask(OFPFlowRemovedFlags mask) { msg_.flowRemovedMask_[0] = mask; }
  void setSlaveFlowRemovedMask(OFPFlowRemovedFlags mask) { msg_.flowRemovedMask_[1] = mask; }

  UInt32 send(Writable *channel);

 private:
  SetAsync msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_SETASYNC_H_
