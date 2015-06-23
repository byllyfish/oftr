// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_FLOWREMOVED_H_
#define OFP_FLOWREMOVED_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/matchbuilder.h"
#include "ofp/tablenumber.h"

namespace ofp {

class FlowRemoved
    : public ProtocolMsg<FlowRemoved, OFPT_FLOW_REMOVED, 56, 65528, true> {
 public:
  UInt64 cookie() const { return cookie_; }
  UInt16 priority() const { return priority_; }
  OFPFlowRemovedReason reason() const { return reason_; }
  TableNumber tableId() const { return tableId_; }
  UInt32 durationSec() const { return durationSec_; }
  UInt32 durationNSec() const { return durationNSec_; }
  UInt16 idleTimeout() const { return idleTimeout_; }
  UInt16 hardTimeout() const { return hardTimeout_; }
  UInt64 packetCount() const { return packetCount_; }
  UInt64 byteCount() const { return byteCount_; }

  Match match() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big64 cookie_;
  Big16 priority_;
  OFPFlowRemovedReason reason_;
  TableNumber tableId_;
  Big32 durationSec_;
  Big32 durationNSec_;
  Big16 idleTimeout_;
  Big16 hardTimeout_;
  Big64 packetCount_;
  Big64 byteCount_;

  MatchHeader matchHeader_;
  Padding<4> pad_2;

  // Only FlowRemovedBuilder can create an instance.
  FlowRemoved() : header_{type()} {}

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 52,
    SizeWithoutMatchHeader = 48
  };

  friend class FlowRemovedBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(FlowRemoved) == 56, "Unexpected size.");
static_assert(IsStandardLayout<FlowRemoved>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<FlowRemoved>(),
              "Expected trivially copyable.");

class FlowRemovedBuilder {
 public:
  FlowRemovedBuilder() = default;
  explicit FlowRemovedBuilder(const FlowRemoved *msg);

  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }
  void setPriority(UInt16 priority) { msg_.priority_ = priority; }
  void setReason(OFPFlowRemovedReason reason) { msg_.reason_ = reason; }
  void setTableId(TableNumber tableId) { msg_.tableId_ = tableId; }
  void setDurationSec(UInt32 durationSec) { msg_.durationSec_ = durationSec; }
  void setDurationNSec(UInt32 durationNSec) {
    msg_.durationNSec_ = durationNSec;
  }
  void setIdleTimeout(UInt16 idleTimeout) { msg_.idleTimeout_ = idleTimeout; }
  void setHardTimeout(UInt16 hardTimeout) { msg_.hardTimeout_ = hardTimeout; }
  void setPacketCount(UInt64 packetCount) { msg_.packetCount_ = packetCount; }
  void setByteCount(UInt64 byteCount) { msg_.byteCount_ = byteCount; }

  void setMatch(const MatchBuilder &match) { match_ = match; }

  UInt32 send(Writable *channel);

 private:
  FlowRemoved msg_;
  MatchBuilder match_;

  UInt32 sendStandard(Writable *channel);
  UInt32 sendOriginal(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_FLOWREMOVED_H_
