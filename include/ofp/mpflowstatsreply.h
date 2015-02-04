// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPFLOWSTATSREPLY_H_
#define OFP_MPFLOWSTATSREPLY_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/match.h"
#include "ofp/instructionrange.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructionlist.h"

namespace ofp {

class Writable;

class MPFlowStatsReply {
 public:
  enum { MPVariableSizeOffset = 0 };

  MPFlowStatsReply() = default;

  UInt8 tableId() const { return tableId_; }
  UInt32 durationSec() const { return durationSec_; }
  UInt32 durationNSec() const { return durationNSec_; }
  UInt16 priority() const { return priority_; }
  UInt16 idleTimeout() const { return idleTimeout_; }
  UInt16 hardTimeout() const { return hardTimeout_; }
  OFPFlowModFlags flags() const { return flags_; }
  UInt64 cookie() const { return cookie_; }
  UInt64 packetCount() const { return packetCount_; }
  UInt64 byteCount() const { return byteCount_; }
  
  Match match() const;
  InstructionRange instructions() const;

  bool validateInput(Validation *context) const;

 private:
  Big16 length_;
  Big8 tableId_;
  Padding<1> pad_1;
  Big32 durationSec_;
  Big32 durationNSec_;
  Big16 priority_;
  Big16 idleTimeout_;
  Big16 hardTimeout_;
  Big<OFPFlowModFlags> flags_;
  Padding<4> pad_2;
  Big64 cookie_;
  Big64 packetCount_;
  Big64 byteCount_;

  MatchHeader matchHeader_;
  Padding<4> pad_3;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 52,
    SizeWithoutMatchHeader = 48
  };

  friend class MPFlowStatsReplyBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPFlowStatsReply) == 56, "Unexpected size.");
static_assert(IsStandardLayout<MPFlowStatsReply>(),
              "Expected standard layout.");

class MPFlowStatsReplyBuilder {
 public:
  void setTableId(UInt8 tableId) { msg_.tableId_ = tableId; }
  void setDurationSec(UInt32 durationSec) { msg_.durationSec_ = durationSec; }
  void setDurationNSec(UInt32 durationNSec) {
    msg_.durationNSec_ = durationNSec;
  }
  void setPriority(UInt16 priority) { msg_.priority_ = priority; }
  void setIdleTimeout(UInt16 idleTimeout) { msg_.idleTimeout_ = idleTimeout; }
  void setHardTimeout(UInt16 hardTimeout) { msg_.hardTimeout_ = hardTimeout; }
  void setFlags(OFPFlowModFlags flags) { msg_.flags_ = flags; }
  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }
  void setPacketCount(UInt64 packetCount) { msg_.packetCount_ = packetCount; }
  void setByteCount(UInt64 byteCount) { msg_.byteCount_ = byteCount; }

  void setMatch(const MatchBuilder &match) { match_ = match; }

  void setInstructions(const InstructionList &instructions) {
    instructions_ = instructions;
  }

  void write(Writable *channel);
  void reset();

 private:
  MPFlowStatsReply msg_;
  MatchBuilder match_;
  InstructionList instructions_;

  void writeV1(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPFLOWSTATSREPLY_H_
