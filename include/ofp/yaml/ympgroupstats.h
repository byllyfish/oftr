// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPGROUPSTATS_H_
#define OFP_YAML_YMPGROUPSTATS_H_

#include "ofp/mpgroupstats.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPGroupStats> {
  static void mapping(IO &io, ofp::MPGroupStats &body) {
    io.mapRequired("group_id", body.groupId_);
    io.mapRequired("ref_count", body.refCount_);
    io.mapRequired("packet_count", body.packetCount_);
    io.mapRequired("byte_count", body.byteCount_);
    io.mapRequired("duration", body.duration_);

    ofp::PacketCounterRange range = body.bucketStats();
    io.mapRequired("bucket_stats", range);
  }
};

template <>
struct MappingTraits<ofp::MPGroupStatsBuilder> {
  static void mapping(IO &io, ofp::MPGroupStatsBuilder &body) {
    io.mapRequired("group_id", body.msg_.groupId_);
    io.mapRequired("ref_count", body.msg_.refCount_);
    io.mapRequired("packet_count", body.msg_.packetCount_);
    io.mapRequired("byte_count", body.msg_.byteCount_);
    io.mapRequired("duration", body.msg_.duration_);

    io.mapRequired("bucket_stats", body.bucketStats_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPGROUPSTATS_H_
