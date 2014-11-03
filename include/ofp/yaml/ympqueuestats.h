//  ===== ---- ofp/yaml/ympqueuestats.h --------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the MappingTraits for MPQueueStats and MPQueueStatsBuilder.
//  ===== ------------------------------------------------------------ =====  //

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
    io.mapRequired("duration_sec", body.durationSec_);
    io.mapRequired("duration_nsec", body.durationNSec_);
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
    io.mapRequired("duration_sec", msg.msg_.durationSec_);
    io.mapRequired("duration_nsec", msg.msg_.durationNSec_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPQUEUESTATS_H_
