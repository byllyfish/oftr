//  ===== ---- ofp/flowstatsreply.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the FlowStatsReply and FlowStatsReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_FLOWSTATSREPLY_H
#define OFP_FLOWSTATSREPLY_H

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/match.h"
#include "ofp/instructionrange.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructionlist.h"

namespace ofp { // <namespace ofp>

class Writable;
class FlowStatsReplyBuilder;

class FlowStatsReply {
public:
  FlowStatsReply() = default;

  Match match() const;
  InstructionRange instructions() const;

private:
  Big16 length_;
  Big8 tableId_;
  Padding<1> pad_1;
  Big32 durationSec_;
  Big32 durationNSec_;
  Big16 priority_;
  Big16 idleTimeout_;
  Big16 hardTimeout_;
  Big16 flags_;
  Padding<4> pad_2;
  Big64 cookie_;
  Big64 packetCount_;
  Big64 byteCount_;

  Big16 matchType_ = 0;
  Big16 matchLength_ = 0;
  Padding<4> pad_3;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 52,
    SizeWithoutMatchHeader = 48,
    MatchHeaderSize = 4
  };

  friend class FlowStatsReplyBuilder;
  friend struct llvm::yaml::MappingTraits<FlowStatsReply>;
  friend struct llvm::yaml::MappingTraits<FlowStatsReplyBuilder>;
};

static_assert(sizeof(FlowStatsReply) == 56, "Unexpected size.");
static_assert(IsStandardLayout<FlowStatsReply>(), "Expected standard layout.");

class FlowStatsReplyBuilder {
public:
  void setTableId(UInt8 tableId) { msg_.tableId_ = tableId; }
  void setDurationSec(UInt32 durationSec) { msg_.durationSec_ = durationSec; }
  void setDurationNSec(UInt32 durationNSec) { msg_.durationNSec_ = durationNSec; }
  void setPriority(UInt16 priority) { msg_.priority_ = priority; }
  void setIdleTimeout(UInt16 idleTimeout) { msg_.idleTimeout_ = idleTimeout; }
  void setHardTimeout(UInt16 hardTimeout) { msg_.hardTimeout_ = hardTimeout; }
  void setFlags(UInt16 flags) { msg_.flags_ = flags; }
  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }
  void setPacketCount(UInt64 packetCount) { msg_.packetCount_ = packetCount; }
  void setByteCount(UInt64 byteCount) { msg_.byteCount_ = byteCount; }

  void setMatch(const MatchBuilder &match) { match_ = match; }

  void setInstructions(const InstructionList &instructions) {
    instructions_ = instructions;
  }

  void write(Writable *channel);

private:
  FlowStatsReply msg_;
  MatchBuilder match_;
  InstructionList instructions_;

  void writeV1(Writable *channel);

  friend struct llvm::yaml::MappingTraits<FlowStatsReplyBuilder>;
};

} // </namespace ofp>

#endif // OFP_FLOWSTATSREPLY_H
