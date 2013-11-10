#ifndef OFP_YAML_YMPPORTSTATS_H
#define OFP_YAML_YMPPORTSTATS_H

#include "ofp/mpportstats.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::MPPortStats> {

  static void mapping(IO &io, ofp::MPPortStats &body) {
    io.mapRequired("port_no", body.portNo_);
    io.mapRequired("rx_packets", body.rxPackets_);
    io.mapRequired("tx_packets", body.txPackets_);
    io.mapRequired("rx_bytes", body.rxBytes_);
    io.mapRequired("tx_bytes", body.txBytes_);
    io.mapRequired("rx_dropped", body.rxDropped_);
    io.mapRequired("tx_dropped", body.txDropped_);
    io.mapRequired("rx_errors", body.rxErrors_);
    io.mapRequired("tx_errors", body.txErrors_);
    io.mapRequired("rx_frame_err", body.rxFrameErr_);
    io.mapRequired("rx_over_err", body.rxOverErr_);
    io.mapRequired("rx_crc_err", body.rxCrcErr_);
    io.mapRequired("collisions", body.collisions_);
    io.mapRequired("duration_sec", body.durationSec_);
    io.mapRequired("duration_nsec", body.durationNSec_);
  }
};

template <>
struct MappingTraits<ofp::MPPortStatsBuilder> {

  static void mapping(IO &io, ofp::MPPortStatsBuilder &msg) {
    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("rx_packets", msg.msg_.rxPackets_);
    io.mapRequired("tx_packets", msg.msg_.txPackets_);
    io.mapRequired("rx_bytes", msg.msg_.rxBytes_);
    io.mapRequired("tx_bytes", msg.msg_.txBytes_);
    io.mapRequired("rx_dropped", msg.msg_.rxDropped_);
    io.mapRequired("tx_dropped", msg.msg_.txDropped_);
    io.mapRequired("rx_errors", msg.msg_.rxErrors_);
    io.mapRequired("tx_errors", msg.msg_.txErrors_);
    io.mapRequired("rx_frame_err", msg.msg_.rxFrameErr_);
    io.mapRequired("rx_over_err", msg.msg_.rxOverErr_);
    io.mapRequired("rx_crc_err", msg.msg_.rxCrcErr_);
    io.mapRequired("collisions", msg.msg_.collisions_);
    io.mapRequired("duration_sec", msg.msg_.durationSec_);
    io.mapRequired("duration_nsec", msg.msg_.durationNSec_);
  }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YMPPORTSTATS_H