#ifndef OFP_YAML_YFLOWSTATSREPLY_H
#define OFP_YAML_YFLOWSTATSREPLY_H

#include "ofp/flowstatsreply.h"
#include "ofp/yaml/yinstructions.h"
#include "ofp/yaml/yflowmod.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::FlowStatsReply> {

    static void mapping(IO &io, ofp::FlowStatsReply &msg)
    {
        using namespace ofp;

        io.mapRequired("table_id", msg.tableId_);
        io.mapRequired("duration_sec", msg.durationSec_);
        io.mapRequired("duration_nsec", msg.durationNsec_);
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
struct MappingTraits<ofp::FlowStatsReplyBuilder> {

    static void mapping(IO &io, ofp::FlowStatsReplyBuilder &msg)
    {
        io.mapRequired("table_id", msg.msg_.tableId_);
        io.mapRequired("duration_sec", msg.msg_.durationSec_);
        io.mapRequired("duration_nsec", msg.msg_.durationNsec_);
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


} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YFLOWSTATSREPLY_H
