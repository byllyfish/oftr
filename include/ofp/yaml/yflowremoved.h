#ifndef OFP_YAML_YFLOWREMOVED_H
#define OFP_YAML_YFLOWREMOVED_H

#include "ofp/flowremoved.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::FlowRemoved> {

    static void mapping(IO &io, ofp::FlowRemoved &msg)
    {
        io.mapRequired("cookie", msg.cookie_);
        io.mapRequired("priority", msg.priority_);
        io.mapRequired("reason", msg.reason_);
        io.mapRequired("table_id", msg.tableId_);
        io.mapRequired("duration_sec", msg.durationSec_);
        io.mapRequired("duration_nsec", msg.durationNSec_);
        io.mapRequired("idle_timeout", msg.idleTimeout_);
        io.mapRequired("hard_timeout", msg.hardTimeout_);
        io.mapRequired("packet_count", msg.packetCount_);
        io.mapRequired("byte_count", msg.byteCount_);

        ofp::Match m = msg.match();
        io.mapRequired("match", m);
    }
};

template <>
struct MappingTraits<ofp::FlowRemovedBuilder> {

    static void mapping(IO &io, ofp::FlowRemovedBuilder &msg)
    {
        io.mapRequired("cookie", msg.msg_.cookie_);
        io.mapRequired("priority", msg.msg_.priority_);
        io.mapRequired("reason", msg.msg_.reason_);
        io.mapRequired("table_id", msg.msg_.tableId_);
        io.mapRequired("duration_sec", msg.msg_.durationSec_);
        io.mapRequired("duration_nsec", msg.msg_.durationNSec_);
        io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
        io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
        io.mapRequired("packet_count", msg.msg_.packetCount_);
        io.mapRequired("byte_count", msg.msg_.byteCount_);

        io.mapRequired("match", msg.match_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YFLOWREMOVED_H
