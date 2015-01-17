// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YFLOWMOD_H_
#define OFP_YAML_YFLOWMOD_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/yinstructions.h"
#include "ofp/flowmod.h"
#include "ofp/match.h"
#include "ofp/yaml/encoder.h"

namespace llvm {
namespace yaml {

/// Use a FLOW_MOD message to modify the flow table in a switch.
///
/// ~~~~~
/// type:     OFPT_FLOW_MOD
/// xid:      UInt64?
/// version:  UInt8?
/// msg:
///   cookie:       UInt64
///   cookie_mask:  UInt64
///   table_id:     UInt8
///   command:      UInt8
///   idle_timeout: UInt16
///   hard_timeout: UInt16
///   priority:     UInt16
///   buffer_id:    UInt32
///   out_port:     UInt32
///   out_group:    UInt32
///   flags:        UInt16
///   match:        [ MatchField* ]
///   instructions: [ Instruction* ]
/// ~~~~~
///

template <>
struct MappingTraits<ofp::FlowMod> {
  static void mapping(IO &io, ofp::FlowMod &msg) {
    io.mapRequired("cookie", msg.cookie_);
    io.mapRequired("cookie_mask", msg.cookieMask_);
    io.mapRequired("table_id", msg.tableId_);
    ofp::OFPFlowModCommand command = msg.command();
    io.mapRequired("command", command);
    io.mapRequired("idle_timeout", msg.idleTimeout_);
    io.mapRequired("hard_timeout", msg.hardTimeout_);
    io.mapRequired("priority", msg.priority_);
    io.mapRequired("buffer_id", msg.bufferId_);
    io.mapRequired("out_port", msg.outPort_);
    io.mapRequired("out_group", msg.outGroup_);
    ofp::OFPFlowModFlags flags = msg.flags();
    io.mapRequired("flags", flags);

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
    ofp::OFPFlowModCommand command;
    io.mapRequired("command", command);
    msg.setCommand(command);
    io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
    io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
    io.mapRequired("priority", msg.msg_.priority_);
    io.mapRequired("buffer_id", msg.msg_.bufferId_);
    io.mapRequired("out_port", msg.msg_.outPort_);
    io.mapRequired("out_group", msg.msg_.outGroup_);
    ofp::OFPFlowModFlags flags;
    io.mapRequired("flags", flags);
    msg.setFlags(flags);
    io.mapRequired("match", msg.match_);
    io.mapRequired("instructions", msg.instructions_);

    ofp::yaml::Encoder *encoder =
        reinterpret_cast<ofp::yaml::Encoder *>(io.getContext());
    if (encoder && encoder->matchPrereqsChecked()) {
      if (!msg.match_.validate()) {
        // TODO(bfish) better error message
        io.setError("Match is ambiguous.");
        ofp::log::info("Match is ambiguous.");
      }
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YFLOWMOD_H_
