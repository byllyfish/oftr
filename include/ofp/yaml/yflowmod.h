// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YFLOWMOD_H_
#define OFP_YAML_YFLOWMOD_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/flowmod.h"
#include "ofp/match.h"
#include "ofp/yaml/encoder.h"
#include "ofp/yaml/ybuffernumber.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/ygroupnumber.h"
#include "ofp/yaml/yinstructions.h"
#include "ofp/yaml/ytablenumber.h"

namespace llvm {
namespace yaml {

const char *const kFlowModSchema = R"""({Message/FlowMod}
type: FLOW_MOD
msg:
  cookie: !opt UInt64               # default=0
  cookie_mask: !opt UInt64          # default=0
  table_id: UInt8
  command: FlowModCommand
  idle_timeout: !opt UInt16         # default=0
  hard_timeout: !opt UInt16         # default=0
  priority: !opt UInt16             # default=0
  buffer_id: !opt BufferNumber      # default=NO_BUFFER
  out_port: !opt PortNumber         # default=ANY
  out_group: !opt GroupNumber       # default=ANY
  flags: !opt [FlowModFlags]        # default=[]
  importance: !optout UInt16        # default=0, min_version=6
  match: !opt [Field]               # default=[]
  instructions: !opt [Instruction]  # default=[]
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
    if (msg.header_.version() >= ofp::OFP_VERSION_6) {
      io.mapRequired("importance", msg.importance_);
    }

    ofp::Match m = msg.match();
    io.mapRequired("match", m);

    ofp::InstructionRange instr = msg.instructions();
    io.mapRequired("instructions", instr);
  }
};

template <>
struct MappingTraits<ofp::FlowModBuilder> {
  static void mapping(IO &io, ofp::FlowModBuilder &msg) {
    io.mapOptional("cookie", msg.msg_.cookie_, 0);
    io.mapOptional("cookie_mask", msg.msg_.cookieMask_, 0);
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapRequired("command", msg.msg_.command_);
    io.mapOptional("idle_timeout", msg.msg_.idleTimeout_, 0);
    io.mapOptional("hard_timeout", msg.msg_.hardTimeout_, 0);
    io.mapOptional("priority", msg.msg_.priority_, 0);
    io.mapOptional("buffer_id", msg.msg_.bufferId_, ofp::OFP_NO_BUFFER);
    io.mapOptional("out_port", msg.msg_.outPort_, ofp::OFPP_ANY);
    io.mapOptional("out_group", msg.msg_.outGroup_, ofp::OFPG_ANY);
    io.mapOptional("flags", msg.msg_.flags_);
    io.mapOptional("importance", msg.msg_.importance_);
    io.mapOptional("match", msg.match_);
    io.mapOptional("instructions", msg.instructions_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YFLOWMOD_H_
