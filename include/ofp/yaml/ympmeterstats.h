// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPMETERSTATS_H_
#define OFP_YAML_YMPMETERSTATS_H_

#include "ofp/mpmeterstats.h"
#include "ofp/yaml/ypacketcounter.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPMeterStats> {
  static void mapping(IO &io, ofp::MPMeterStats &body) {
    io.mapRequired("meter_id", body.meterId_);
    io.mapRequired("flow_count", body.flowCount_);
    io.mapRequired("packet_in_count", body.packetInCount_);
    io.mapRequired("byte_in_count", body.byteInCount_);
    io.mapRequired("duration", body.duration_);

    ofp::PacketCounterRange bands = body.bandStats();
    io.mapRequired("bands", bands);
  }
};

template <>
struct MappingTraits<ofp::MPMeterStatsBuilder> {
  static void mapping(IO &io, ofp::MPMeterStatsBuilder &body) {
    io.mapRequired("meter_id", body.msg_.meterId_);
    io.mapRequired("flow_count", body.msg_.flowCount_);
    io.mapRequired("packet_in_count", body.msg_.packetInCount_);
    io.mapRequired("byte_in_count", body.msg_.byteInCount_);
    io.mapRequired("duration", body.msg_.duration_);

    io.mapRequired("bands", body.bandStats_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPMETERSTATS_H_
