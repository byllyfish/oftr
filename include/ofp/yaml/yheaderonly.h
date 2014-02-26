//  ===== ---- ofp/yaml/yheaderonly.h ----------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the header only classes:
/// FeaturesRequest, FeaturesRequestBuilder, GetAsyncRequest,
/// GetAsyncRequestBuilder, GetConfigRequest, GetConfigRequestBuilder,
/// BarrierRequest, BarrierRequestBuilder, BarrierReply, BarrierReplyBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YHEADERONLY_H_
#define OFP_YAML_YHEADERONLY_H_

#include "ofp/headeronly.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_FEATURES_REQUEST
// msg:
//...

template <>
struct MappingTraits<ofp::FeaturesRequest> {
  static void mapping(IO &io, ofp::FeaturesRequest &msg) {}
};

template <>
struct MappingTraits<ofp::FeaturesRequestBuilder> {
  static void mapping(IO &io, ofp::FeaturesRequestBuilder &msg) {}
};

//---
// type: OFPT_GET_ASYNC_REQUEST
// msg:
//...

template <>
struct MappingTraits<ofp::GetAsyncRequest> {
  static void mapping(IO &io, ofp::GetAsyncRequest &msg) {}
};

template <>
struct MappingTraits<ofp::GetAsyncRequestBuilder> {
  static void mapping(IO &io, ofp::GetAsyncRequestBuilder &msg) {}
};

//---
// type: OFPT_GET_CONFIG_REQUEST
// msg:
//...

template <>
struct MappingTraits<ofp::GetConfigRequest> {
  static void mapping(IO &io, ofp::GetConfigRequest &msg) {}
};

template <>
struct MappingTraits<ofp::GetConfigRequestBuilder> {
  static void mapping(IO &io, ofp::GetConfigRequestBuilder &msg) {}
};

//---
// type: OFPT_BARRIER_REQUEST
// msg:
//...

template <>
struct MappingTraits<ofp::BarrierRequest> {
  static void mapping(IO &io, ofp::BarrierRequest &msg) {}
};

template <>
struct MappingTraits<ofp::BarrierRequestBuilder> {
  static void mapping(IO &io, ofp::BarrierRequestBuilder &msg) {}
};

//---
// type: OFPT_BARRIER_REPLY
// msg:
//...

template <>
struct MappingTraits<ofp::BarrierReply> {
  static void mapping(IO &io, ofp::BarrierReply &msg) {}
};

template <>
struct MappingTraits<ofp::BarrierReplyBuilder> {
  static void mapping(IO &io, ofp::BarrierReplyBuilder &msg) {}
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YHEADERONLY_H_
