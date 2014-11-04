// Copyright 2014-present Bill Fisher. All rights reserved.

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
