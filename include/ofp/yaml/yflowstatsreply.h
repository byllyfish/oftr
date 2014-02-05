//  ===== ---- ofp/yaml/yflowstatsreply.h ------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the MPFlowStatsReply and
/// MPFlowStatsReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YFLOWSTATSREPLY_H_
#define OFP_YAML_YFLOWSTATSREPLY_H_

#include "ofp/mpflowstatsreply.h"
#include "ofp/yaml/yinstructions.h"
#include "ofp/yaml/yflowmod.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPFlowStatsReply> {

    static void mapping(IO &io, ofp::MPFlowStatsReply &msg)
    {
        using namespace ofp;

        io.mapRequired("table_id", msg.tableId_);
        io.mapRequired("duration_sec", msg.durationSec_);
        io.mapRequired("duration_nsec", msg.durationNSec_);
        io.mapRequired("priority", msg.priority_);
        io.mapRequired("idle_timeout", msg.idleTimeout_);
        io.mapRequired("hard_timeout", msg.hardTimeout_);
        io.mapRequired("flags", msg.flags_);
        io.mapRequired("cookie", msg.cookie_);
        io.mapRequired("packet_count", msg.packetCount_);
        io.mapRequired("byte_count", msg.byteCount_);

        Match m = msg.match();
        io.mapRequired("match", m);

        InstructionRange instrs = msg.instructions();
        io.mapRequired("instructions", instrs);
    }
};


template <>
struct MappingTraits<ofp::MPFlowStatsReplyBuilder> {

    static void mapping(IO &io, ofp::MPFlowStatsReplyBuilder &msg)
    {
        io.mapRequired("table_id", msg.msg_.tableId_);
        io.mapRequired("duration_sec", msg.msg_.durationSec_);
        io.mapRequired("duration_nsec", msg.msg_.durationNSec_);
        io.mapRequired("priority", msg.msg_.priority_);
        io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
        io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
        io.mapRequired("flags", msg.msg_.flags_);
        io.mapRequired("cookie", msg.msg_.cookie_);
        io.mapRequired("packet_count", msg.msg_.packetCount_);
        io.mapRequired("byte_count", msg.msg_.byteCount_);
        io.mapRequired("match", msg.match_);
        io.mapRequired("instructions", msg.instructions_);
    }
};


}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YFLOWSTATSREPLY_H_
