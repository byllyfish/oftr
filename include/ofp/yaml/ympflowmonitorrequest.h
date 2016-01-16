// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPFLOWMONITORREQUEST_H_
#define OFP_YAML_YMPFLOWMONITORREQUEST_H_

#include "ofp/mpflowmonitorrequest.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPFlowMonitorRequest> {
  static void mapping(IO &io, ofp::MPFlowMonitorRequest &msg) {
    io.mapRequired("monitor_id", msg.monitorId_);
    io.mapRequired("out_port", msg.outPort_);
    io.mapRequired("out_group", msg.outGroup_);
    io.mapRequired("flags", msg.flags_);
    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("command", msg.command_);

    ofp::Match m = msg.match();
    io.mapRequired("match", m);
  }
};

template <>
struct MappingTraits<ofp::MPFlowMonitorRequestBuilder> {
  static void mapping(IO &io, ofp::MPFlowMonitorRequestBuilder &msg) {
    io.mapRequired("monitor_id", msg.msg_.monitorId_);
    io.mapRequired("out_port", msg.msg_.outPort_);
    io.mapRequired("out_group", msg.msg_.outGroup_);
    io.mapRequired("flags", msg.msg_.flags_);
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapRequired("command", msg.msg_.command_);

    io.mapRequired("match", msg.match_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPFLOWMONITORREQUEST_H_
