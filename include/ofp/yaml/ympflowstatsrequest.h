// Copyright 2014-present Bill Fisher. All rights reserved.

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
