// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPFLOWSTATSREPLY_H_
#define OFP_YAML_YMPFLOWSTATSREPLY_H_

#include "ofp/mpflowstatsreply.h"
#include "ofp/yaml/ydurationsec.h"
#include "ofp/yaml/yflowmod.h"
#include "ofp/yaml/yinstructions.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPFlowStatsReply> {
  static void mapping(IO &io, ofp::MPFlowStatsReply &msg) {
    using namespace ofp;

    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("duration", msg.duration_);
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
  static void mapping(IO &io, ofp::MPFlowStatsReplyBuilder &msg) {
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapRequired("duration", msg.msg_.duration_);
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

#endif  // OFP_YAML_YMPFLOWSTATSREPLY_H_
