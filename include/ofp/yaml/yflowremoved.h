// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YFLOWREMOVED_H_
#define OFP_YAML_YFLOWREMOVED_H_

#include "ofp/flowremoved.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_FLOW_REMOVED
// msg:
//   cookie: <UInt64>           { Required }
//   priority: <UInt16>         { Required }
//   reason: <UInt8>            { Required }
//   table_id: <UInt8>          { Required }
//   duration_sec: <UInt32>     { Required }
//   duration_nsec: <UInt32>    { Required }
//   idle_timeout: <UInt16>     { Required }
//   hard_timeout: <UInt16>     { Required }
//   packet_count: <UInt64>     { Required }
//   byte_count: <UInt64>       { Required }
//   match: [ <MatchItem> ]     { Required }
//...

template <>
struct MappingTraits<ofp::FlowRemoved> {
  static void mapping(IO &io, ofp::FlowRemoved &msg) {
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
  static void mapping(IO &io, ofp::FlowRemovedBuilder &msg) {
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

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YFLOWREMOVED_H_
