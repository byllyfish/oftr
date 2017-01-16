// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPGROUPSTATSREQUEST_H_
#define OFP_YAML_YMPGROUPSTATSREQUEST_H_

#include "ofp/mpgroupstatsrequest.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPGroupStatsRequest> {
  static void mapping(IO &io, ofp::MPGroupStatsRequest &body) {
    io.mapRequired("group_id", body.groupId_);
  }
};

template <>
struct MappingTraits<ofp::MPGroupStatsRequestBuilder> {
  static void mapping(IO &io, ofp::MPGroupStatsRequestBuilder &body) {
    io.mapRequired("group_id", body.msg_.groupId_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPGROUPSTATSREQUEST_H_
