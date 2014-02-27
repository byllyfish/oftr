//  ===== ---- ofp/yaml/ympqueuestatsrequest.h -------------*- C++ -*- =====  //
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
/// \brief Defines the MappingTraits for MPQueueStatsRequest and 
/// MPQueueStatsRequestBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YMPQUEUESTATSREQUEST_H_
#define OFP_YAML_YMPQUEUESTATSREQUEST_H_

#include "ofp/mpqueuestatsrequest.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPQueueStatsRequest> {

  static void mapping(IO &io, ofp::MPQueueStatsRequest &body) {
    io.mapRequired("port_no", body.portNo_);
    io.mapRequired("queue_id", body.queueId_);
  }
};

template <>
struct MappingTraits<ofp::MPQueueStatsRequestBuilder> {

  static void mapping(IO &io, ofp::MPQueueStatsRequestBuilder &msg) {
    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("queue_id", msg.msg_.queueId_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPQUEUESTATSREQUEST_H_
