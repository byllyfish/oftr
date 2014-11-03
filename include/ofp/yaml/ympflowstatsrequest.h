//  ===== ---- ofp/yaml/yflowstatsrequest.h ----------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the MPFlowStatsRequest and
/// MPFlowStatsRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YMPFLOWSTATSREQUEST_H_
#define OFP_YAML_YMPFLOWSTATSREQUEST_H_

#include "ofp/mpflowstatsrequest.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPFlowStatsRequest> {
  static void mapping(IO &io, ofp::MPFlowStatsRequest &msg) {
    using namespace ofp;

    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("out_port", msg.outPort_);
    io.mapRequired("out_group", msg.outGroup_);
    io.mapRequired("cookie", msg.cookie_);
    io.mapRequired("cookie_mask", msg.cookieMask_);

    Match m = msg.match();
    io.mapRequired("match", m);
  }
};

template <>
struct MappingTraits<ofp::MPFlowStatsRequestBuilder> {
  static void mapping(IO &io, ofp::MPFlowStatsRequestBuilder &msg) {
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapRequired("out_port", msg.msg_.outPort_);
    io.mapRequired("out_group", msg.msg_.outGroup_);
    io.mapRequired("cookie", msg.msg_.cookie_);
    io.mapRequired("cookie_mask", msg.msg_.cookieMask_);
    io.mapRequired("match", msg.match_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPFLOWSTATSREQUEST_H_
