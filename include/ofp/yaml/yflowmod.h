// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YFLOWMOD_H_
#define OFP_YAML_YFLOWMOD_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/yinstructions.h"
#include "ofp/yaml/ybuffernumber.h"
#include "ofp/yaml/ygroupnumber.h"
#include "ofp/yaml/ytablenumber.h"
#include "ofp/flowmod.h"
#include "ofp/match.h"
#include "ofp/yaml/encoder.h"

namespace llvm {
namespace yaml {

const char *const kFlowModSchema = R"""({Message/FlowMod}
type: FLOW_MOD
msg:
  cookie: UInt64
  cookie_mask: UInt64
  table_id: UInt8
  command: FlowModCommand
  idle_timeout: UInt16
  hard_timeout: UInt16
  priority: UInt16
  buffer_id: BufferNumber
  out_port: PortNumber
  out_group: GroupNumber
  flags: [FlowModFlags]
  match: [Field]
  instructions: [Instruction]
)""";

template <>
struct MappingTraits<ofp::FlowMod> {
  static void mapping(IO &io, ofp::FlowMod &msg) {
    io.mapRequired("cookie", msg.cookie_);
    io.mapRequired("cookie_mask", msg.cookieMask_);
    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("command", msg.command_);
    io.mapRequired("idle_timeout", msg.idleTimeout_);
    io.mapRequired("hard_timeout", msg.hardTimeout_);
    io.mapRequired("priority", msg.priority_);
    io.mapRequired("buffer_id", msg.bufferId_);
    io.mapRequired("out_port", msg.outPort_);
    io.mapRequired("out_group", msg.outGroup_);
    io.mapRequired("flags", msg.flags_);

    ofp::Match m = msg.match();
    io.mapRequired("match", m);

    ofp::InstructionRange instr = msg.instructions();
    io.mapRequired("instructions", instr);
  }
};

template <>
struct MappingTraits<ofp::FlowModBuilder> {
  static void mapping(IO &io, ofp::FlowModBuilder &msg) {
    io.mapRequired("cookie", msg.msg_.cookie_);
    io.mapRequired("cookie_mask", msg.msg_.cookieMask_);
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapRequired("command", msg.msg_.command_);
    io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
    io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
    io.mapRequired("priority", msg.msg_.priority_);
    io.mapRequired("buffer_id", msg.msg_.bufferId_);
    io.mapRequired("out_port", msg.msg_.outPort_);
    io.mapRequired("out_group", msg.msg_.outGroup_);
    io.mapRequired("flags", msg.msg_.flags_);
    io.mapRequired("match", msg.match_);
    io.mapRequired("instructions", msg.instructions_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YFLOWMOD_H_
