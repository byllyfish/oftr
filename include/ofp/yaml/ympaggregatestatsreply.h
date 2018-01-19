// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPAGGREGATESTATSREPLY_H_
#define OFP_YAML_YMPAGGREGATESTATSREPLY_H_

#include "ofp/mpaggregatestatsreply.h"
#include "ofp/mpaggregatestatsreplyv6.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPAggregateStatsReply> {
  static void mapping(IO &io, ofp::MPAggregateStatsReply &msg) {
    io.mapRequired("packet_count", msg.packetCount_);
    io.mapRequired("byte_count", msg.byteCount_);
    io.mapRequired("flow_count", msg.flowCount_);
  }
};

template <>
struct MappingTraits<ofp::MPAggregateStatsReplyBuilder> {
  static void mapping(IO &io, ofp::MPAggregateStatsReplyBuilder &msg) {
    io.mapRequired("packet_count", msg.msg_.packetCount_);
    io.mapRequired("byte_count", msg.msg_.byteCount_);
    io.mapRequired("flow_count", msg.msg_.flowCount_);
  }
};

template <>
struct MappingTraits<ofp::MPAggregateStatsReplyV6> {
  static void mapping(IO &io, ofp::MPAggregateStatsReplyV6 &msg) {
    Hex64 packetCount = msg.packetCount();
    Hex64 byteCount = msg.byteCount();
    Hex32 flowCount = msg.flowCount();

    io.mapRequired("packet_count", packetCount);
    io.mapRequired("byte_count", byteCount);
    io.mapRequired("flow_count", flowCount);
  }
};

template <>
struct MappingTraits<ofp::MPAggregateStatsReplyV6Builder> {
  static void mapping(IO &io, ofp::MPAggregateStatsReplyV6Builder &msg) {
    using namespace ofp;

    UInt64 packetCount = 0;
    UInt64 byteCount = 0;
    UInt32 flowCount = 0;

    io.mapRequired("packet_count", packetCount);
    io.mapRequired("byte_count", byteCount);
    io.mapRequired("flow_count", flowCount);

    msg.setPacketCount(packetCount);
    msg.setByteCount(byteCount);
    msg.setFlowCount(flowCount);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPAGGREGATESTATSREPLY_H_
