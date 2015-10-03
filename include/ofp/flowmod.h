// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_FLOWMOD_H_
#define OFP_FLOWMOD_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"
#include "ofp/matchheader.h"
#include "ofp/instructionlist.h"
#include "ofp/instructionrange.h"
#include "ofp/buffernumber.h"
#include "ofp/groupnumber.h"
#include "ofp/tablenumber.h"

namespace ofp {

class FlowMod : public ProtocolMsg<FlowMod, OFPT_FLOW_MOD, 56, 65528> {
 public:
  UInt64 cookie() const { return cookie_; }
  UInt64 cookieMask() const { return cookieMask_; }
  TableNumber tableId() const { return tableId_; }
  OFPFlowModCommand command() const { return command_; }
  UInt16 idleTimeout() const { return idleTimeout_; }
  UInt16 hardTimeout() const { return hardTimeout_; }
  UInt16 priority() const { return priority_; }
  BufferNumber bufferId() const { return bufferId_; }
  PortNumber outPort() const { return outPort_; }
  GroupNumber outGroup() const { return outGroup_; }
  OFPFlowModFlags flags() const { return flags_; }

  Match match() const;
  InstructionRange instructions() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big64 cookie_ = 0;
  Big64 cookieMask_ = 0;
  TableNumber tableId_ = 0;
  Big<OFPFlowModCommand> command_ = OFPFC_ADD;
  Big16 idleTimeout_ = 0;
  Big16 hardTimeout_ = 0;
  Big16 priority_ = 0;
  BufferNumber bufferId_ = 0;
  PortNumber outPort_ = 0;
  GroupNumber outGroup_ = 0;
  Big<OFPFlowModFlags> flags_ = OFPFF_NONE;
  Padding<2> pad_1;

  MatchHeader matchHeader_;
  Padding<4> pad_2;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 52,
    SizeWithoutMatchHeader = 48
  };

  // Only FlowModBuilder can construct an instance.
  FlowMod() : header_{type()} {}

  friend class FlowModBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(FlowMod) == 56, "Unexpected size.");
static_assert(IsStandardLayout<FlowMod>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<FlowMod>(), "Expected trivially copyable.");

class FlowModBuilder {
 public:
  FlowModBuilder() = default;
  explicit FlowModBuilder(const FlowMod *msg);

  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }
  void setCookieMask(UInt64 cookieMask) { msg_.cookieMask_ = cookieMask; }
  void setTableId(TableNumber tableId) { msg_.tableId_ = tableId; }
  void setCommand(OFPFlowModCommand command) { msg_.command_ = command; }
  void setIdleTimeout(UInt16 idleTimeout) { msg_.idleTimeout_ = idleTimeout; }
  void setHardTimeout(UInt16 hardTimeout) { msg_.hardTimeout_ = hardTimeout; }
  void setPriority(UInt16 priority) { msg_.priority_ = priority; }
  void setBufferId(BufferNumber bufferId) { msg_.bufferId_ = bufferId; }
  void setOutPort(PortNumber outPort) { msg_.outPort_ = outPort; }
  void setOutGroup(GroupNumber outGroup) { msg_.outGroup_ = outGroup; }
  void setFlags(OFPFlowModFlags flags) { msg_.flags_ = flags; }

  MatchBuilder &match() { return match_; }

  void setMatch(const MatchBuilder &match) { match_ = match; }

  void setMatch(MatchBuilder &&match) { match_ = std::move(match); }

  InstructionList &instructions() { return instructions_; }

  void setInstructions(const InstructionList &instructions) {
    instructions_ = instructions;
  }

  void setInstructions(InstructionList &&instructions) {
    instructions_ = std::move(instructions);
  }

  UInt32 send(Writable *channel);

  static UInt32 sendFastVersion1(Writable *channel, UInt32 inPort,
                                 UInt32 outPort, UInt32 bufferId,
                                 const MacAddress &dst,
                                 const MacAddress &src);

 private:
  FlowMod msg_;
  MatchBuilder match_;
  InstructionList instructions_;

  UInt32 sendStandard(Writable *channel);
  UInt32 sendOriginal(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_FLOWMOD_H_
