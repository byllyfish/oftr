//  ===== ---- ofp/yaml/yflowmod.h -------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the FlowMod and
/// FlowModBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YFLOWMOD_H_
#define OFP_YAML_YFLOWMOD_H_

//#include "ofp/yaml.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/yinstructions.h"
#include "ofp/flowmod.h"
#include "ofp/match.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_FLOW_MOD
// msg:
//   cookie: <UInt64>                   { Required }
//   cookie_mask: <UInt64>              { Required }
//   table_id: <UInt8>                  { Required }
//   command:  <UInt8>                  { Required }
//   idle_timeout: <UInt16>             { Required }
//   hard_timeout: <UInt16>             { Required }
//   priority: <UInt16>                 { Required }
//   buffer_id: <UInt32>                { Required }
//   out_port: <UInt32>                 { Required }
//   out_group: <UInt32>                { Required }
//   flags: <UInt16>                    { Required }
//   match: [ <MatchItem> ]             { Required }
//   instructions: [ <Instruction> ]    { Required }
//...

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
    ofp::OFPFlowModCommand command;
    io.mapRequired("command", command);
    msg.setCommand(command);
    io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
    io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
    io.mapRequired("priority", msg.msg_.priority_);
    io.mapRequired("buffer_id", msg.msg_.bufferId_);
    io.mapRequired("out_port", msg.msg_.outPort_);
    io.mapRequired("out_group", msg.msg_.outGroup_);
    io.mapRequired("flags", msg.msg_.flags_);

    io.mapRequired("match", msg.match_);
    io.mapRequired("instructions", msg.instructions_);

    if (!msg.match_.validate()) {
      // TODO(bfish) better error message
      io.setError("Match is ambiguous.");
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YFLOWMOD_H_
