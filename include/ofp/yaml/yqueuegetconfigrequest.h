//  ===== ---- ofp/yqueuegetconfigrequest.h ----------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for QueueGetConfigRequest and
/// QueueGetConfigRequestBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YQUEUEGETCONFIGREQUEST_H_
#define OFP_YAML_YQUEUEGETCONFIGREQUEST_H_

#include "ofp/queuegetconfigrequest.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_QUEUE_GET_CONFIG_REQUEST
// msg:
//   port: <UInt32>    { Required }
//...

template <>
struct MappingTraits<ofp::QueueGetConfigRequest> {

  static void mapping(IO &io, ofp::QueueGetConfigRequest &msg) {
    io.mapRequired("port", msg.port_);
  }
};

template <>
struct MappingTraits<ofp::QueueGetConfigRequestBuilder> {

  static void mapping(IO &io, ofp::QueueGetConfigRequestBuilder &msg) {
    io.mapRequired("port", msg.msg_.port_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YQUEUEGETCONFIGREQUEST_H_
