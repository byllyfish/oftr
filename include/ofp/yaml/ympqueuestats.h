// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPQUEUESTATS_H_
#define OFP_YAML_YMPQUEUESTATS_H_

#include "ofp/mpqueuestats.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPQueueStats> {
  static void mapping(IO &io, ofp::MPQueueStats &body) {
    io.mapRequired("port_no", body.portNo_);
    io.mapRequired("queue_id", body.queueId_);
    io.mapRequired("tx_packets", body.txPackets_);
    io.mapRequired("tx_bytes", body.txBytes_);
    io.mapRequired("tx_errors", body.txErrors_);
    io.mapRequired("duration", body.duration_);
  }
};

template <>
struct MappingTraits<ofp::MPQueueStatsBuilder> {
  static void mapping(IO &io, ofp::MPQueueStatsBuilder &msg) {
    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("queue_id", msg.msg_.queueId_);
    io.mapRequired("tx_packets", msg.msg_.txPackets_);
    io.mapRequired("tx_bytes", msg.msg_.txBytes_);
    io.mapRequired("tx_errors", msg.msg_.txErrors_);
    io.mapRequired("duration", msg.msg_.duration_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPQUEUESTATS_H_
