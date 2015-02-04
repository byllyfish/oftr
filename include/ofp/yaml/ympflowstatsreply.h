// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YMPFLOWSTATSREPLY_H_
#define OFP_YAML_YMPFLOWSTATSREPLY_H_

#include "ofp/mpflowstatsreply.h"
#include "ofp/yaml/yinstructions.h"
#include "ofp/yaml/yflowmod.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPFlowStatsReply> {
  static void mapping(IO &io, ofp::MPFlowStatsReply &msg) {
    using namespace ofp;

    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("duration_sec", msg.durationSec_);
    io.mapRequired("duration_nsec", msg.durationNSec_);
    io.mapRequired("priority", msg.priority_);
    io.mapRequired("idle_timeout", msg.idleTimeout_);
    io.mapRequired("hard_timeout", msg.hardTimeout_);
    OFPFlowModFlags flags = msg.flags();
    io.mapRequired("flags", flags);
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
    io.mapRequired("duration_sec", msg.msg_.durationSec_);
    io.mapRequired("duration_nsec", msg.msg_.durationNSec_);
    io.mapRequired("priority", msg.msg_.priority_);
    io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
    io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
    ofp::OFPFlowModFlags flags;
    io.mapRequired("flags", flags);
    msg.setFlags(flags);
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
