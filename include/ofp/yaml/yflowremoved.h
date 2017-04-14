// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YFLOWREMOVED_H_
#define OFP_YAML_YFLOWREMOVED_H_

#include "ofp/flowremoved.h"
#include "ofp/flowremovedv6.h"
#include "ofp/yaml/ydurationsec.h"
#include "ofp/yaml/ystat.h"

namespace llvm {
namespace yaml {

const char *const kFlowRemovedSchema = R"""({Message/FlowRemoved}
type: FLOW_REMOVED
msg:
  cookie: UInt64
  priority: UInt16
  reason: FlowRemovedReason
  table_id: TableNumber
  duration: DurationSec
  idle_timeout: UInt16
  hard_timeout: UInt16
  packet_count: UInt64
  byte_count: UInt64
  match: [Field]
  stat: [Field]
)""";

template <>
struct MappingTraits<ofp::FlowRemoved> {
  static void mapping(IO &io, ofp::FlowRemoved &msg) {
    io.mapRequired("cookie", msg.cookie_);
    io.mapRequired("priority", msg.priority_);
    io.mapRequired("reason", msg.reason_);
    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("duration", msg.duration_);
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
    io.mapRequired("duration", msg.msg_.duration_);
    io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
    io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
    io.mapRequired("packet_count", msg.msg_.packetCount_);
    io.mapRequired("byte_count", msg.msg_.byteCount_);

    io.mapRequired("match", msg.match_);
  }
};

template <>
struct MappingTraits<ofp::FlowRemovedV6> {
  static void mapping(IO &io, ofp::FlowRemovedV6 &msg) {
    using namespace ofp;
    DurationSec duration = msg.duration();
    UInt64 packetCount = msg.packetCount();
    UInt64 byteCount = msg.byteCount();

    io.mapRequired("cookie", msg.cookie_);
    io.mapRequired("priority", msg.priority_);
    io.mapRequired("reason", msg.reason_);
    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("duration", duration);
    io.mapRequired("idle_timeout", msg.idleTimeout_);
    io.mapRequired("hard_timeout", msg.hardTimeout_);
    io.mapRequired("packet_count", packetCount);
    io.mapRequired("byte_count", byteCount);

    Match m = msg.match();
    io.mapRequired("match", m);

    Stat s = msg.stat();
    io.mapRequired("stat", m);
  }
};

template <>
struct MappingTraits<ofp::FlowRemovedV6Builder> {
  static void mapping(IO &io, ofp::FlowRemovedV6Builder &msg) {
    using namespace ofp;

    DurationSec duration;
    UInt64 packetCount = 0;
    UInt64 byteCount = 0;

    io.mapRequired("cookie", msg.msg_.cookie_);
    io.mapRequired("priority", msg.msg_.priority_);
    io.mapRequired("reason", msg.msg_.reason_);
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapRequired("duration", duration);
    io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
    io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
    io.mapRequired("packet_count", packetCount);
    io.mapRequired("byte_count", byteCount);

    msg.setDuration(duration);
    msg.setPacketCount(packetCount);
    msg.setByteCount(byteCount);

    io.mapRequired("match", msg.match_);
    io.mapRequired("stat", msg.stat_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YFLOWREMOVED_H_
