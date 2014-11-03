//  ===== ---- ofp/yaml/ympaggregatestatsreply.h -----------*- C++ -*- =====  //
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
/// \brief Defines the MappingTraits classes for MPAggregateStatsReply and
/// MPAggregateStatsReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YMPAGGREGATESTATSREPLY_H_
#define OFP_YAML_YMPAGGREGATESTATSREPLY_H_

#include "ofp/mpaggregatestatsreply.h"

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

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPAGGREGATESTATSREPLY_H_
