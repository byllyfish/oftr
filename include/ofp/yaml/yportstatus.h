// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YPORTSTATUS_H_
#define OFP_YAML_YPORTSTATUS_H_

#include "ofp/portstatus.h"
#include "ofp/yaml/yport.h"

namespace llvm {
namespace yaml {

const char *const kPortStatusSchema = R"""({Message/PortStatus}
type: PORT_STATUS
msg:
  reason: PortStatusReason
  port_no: PortNumber
  hw_addr: MacAddress
  name: Str16
  config: [PortConfigFlags]
  state: [PortStateFlags]
  curr: [PortFeaturesFlags]
  advertised: [PortFeaturesFlags]
  supported: [PortFeaturesFlags]
  peer: [PortFeaturesFlags]
  curr_speed: UInt32
  max_speed: UInt32
  optical: !optout
    supported: [OpticalPortFeaturesFlags]
    tx_min_freq_lmda: UInt32
    tx_max_freq_lmda: UInt32
    tx_grid_freq_lmda: UInt32
    rx_min_freq_lmda: UInt32
    rx_max_freq_lmda: UInt32
    rx_grid_freq_lmda: UInt32
    tx_pwr_min: UInt16
    tx_pwr_max: UInt16
  properties: !opt [ExperimenterProperty]
)""";

template <>
struct MappingTraits<ofp::PortStatus> {
  static void mapping(IO &io, ofp::PortStatus &msg) {
    io.mapRequired("reason", msg.reason_);

    ofp::Port &port = RemoveConst_cast(msg.port());
    MappingTraits<ofp::Port>::mapping(io, port);
  }
};

template <>
struct MappingTraits<ofp::PortStatusBuilder> {
  static void mapping(IO &io, ofp::PortStatusBuilder &msg) {
    io.mapRequired("reason", msg.msg_.reason_);
    MappingTraits<ofp::PortBuilder>::mapping(io, msg.port_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORTSTATUS_H_
