//  ===== ---- ofp/flowmod.h -------------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the FlowMod and FlowModBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_FLOWMOD_H_
#define OFP_FLOWMOD_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"
#include "ofp/matchheader.h"
#include "ofp/instructionlist.h"
#include "ofp/instructionrange.h"

namespace ofp {

class FlowMod : public ProtocolMsg<FlowMod, OFPT_FLOW_MOD, 56, 65528> {
 public:
  UInt64 cookie() const { return cookie_; }
  UInt64 cookieMask() const { return cookieMask_; }
  UInt8 tableId() const { return tableId_; }
  OFPFlowModCommand command() const { return command_; }
  UInt16 idleTimeout() const { return idleTimeout_; }
  UInt16 hardTimeout() const { return hardTimeout_; }
  UInt16 priority() const { return priority_; }
  UInt32 bufferId() const { return bufferId_; }
  UInt32 outPort() const { return outPort_; }
  UInt32 outGroup() const { return outGroup_; }
  UInt16 flags() const { return flags_; }

  Match match() const;
  InstructionRange instructions() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big64 cookie_ = 0;
  Big64 cookieMask_ = 0;
  Big8 tableId_ = 0;
  Big<OFPFlowModCommand> command_ = OFPFC_ADD;
  Big16 idleTimeout_ = 0;
  Big16 hardTimeout_ = 0;
  Big16 priority_ = 0;
  Big32 bufferId_ = 0;
  Big32 outPort_ = 0;
  Big32 outGroup_ = 0;
  Big16 flags_ = 0;
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
  void setTableId(UInt8 tableId) { msg_.tableId_ = tableId; }
  void setCommand(OFPFlowModCommand command) { msg_.command_ = command; }
  void setIdleTimeout(UInt16 idleTimeout) { msg_.idleTimeout_ = idleTimeout; }
  void setHardTimeout(UInt16 hardTimeout) { msg_.hardTimeout_ = hardTimeout; }
  void setPriority(UInt16 priority) { msg_.priority_ = priority; }
  void setBufferId(UInt32 bufferId) { msg_.bufferId_ = bufferId; }
  void setOutPort(UInt32 outPort) { msg_.outPort_ = outPort; }
  void setOutGroup(UInt32 outGroup) { msg_.outGroup_ = outGroup; }
  void setFlags(UInt16 flags) { msg_.flags_ = flags; }

  MatchBuilder &match() { return match_; }

  void setMatch(const MatchBuilder &match) {
    assert(match.validate());
    match_ = match;
  }

  void setMatch(MatchBuilder &&match) {
    assert(match.validate());
    match_ = std::move(match);
  }

  InstructionList &instructions() { return instructions_; }
  
  void setInstructions(const InstructionList &instructions) {
    instructions_ = instructions;
  }

  void setInstructions(InstructionList &&instructions) {
    instructions_ = std::move(instructions);
  }

  UInt32 send(Writable *channel);

  static UInt32 sendFastVersion1(Writable *channel, UInt32 inPort, UInt32 outPort, UInt32 bufferId,
                    const EnetAddress &dst, const EnetAddress &src);

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
