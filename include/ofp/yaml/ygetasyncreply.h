// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YGETASYNCREPLY_H_
#define OFP_YAML_YGETASYNCREPLY_H_

#include "ofp/getasyncreply.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_GET_ASYNC_REPLY
// msg:
//   packet_in_mask_master: <UInt32>       { Required }
//   packet_in_mask_slave: <UInt32>        { Required }
//   port_status_mask_master: <UInt32>     { Required }
//   port_status_mask_slave: <UInt32>      { Required }
//   flow_removed_mask_master: <UInt32>    { Required }
//   flow_removed_mask_slave: <UInt32>     { Required }
//...

template <>
struct MappingTraits<ofp::GetAsyncReply> {
  static void mapping(IO &io, ofp::GetAsyncReply &msg) {
    io.mapRequired("packet_in_mask_master", msg.packetInMask_[0]);
    io.mapRequired("packet_in_mask_slave", msg.packetInMask_[1]);
    io.mapRequired("port_status_mask_master", msg.portStatusMask_[0]);
    io.mapRequired("port_status_mask_slave", msg.portStatusMask_[1]);
    io.mapRequired("flow_removed_mask_master", msg.flowRemovedMask_[0]);
    io.mapRequired("flow_removed_mask_slave", msg.flowRemovedMask_[1]);
  }
};

template <>
struct MappingTraits<ofp::GetAsyncReplyBuilder> {
  static void mapping(IO &io, ofp::GetAsyncReplyBuilder &msg) {
    io.mapRequired("packet_in_mask_master", msg.msg_.packetInMask_[0]);
    io.mapRequired("packet_in_mask_slave", msg.msg_.packetInMask_[1]);
    io.mapRequired("port_status_mask_master", msg.msg_.portStatusMask_[0]);
    io.mapRequired("port_status_mask_slave", msg.msg_.portStatusMask_[1]);
    io.mapRequired("flow_removed_mask_master", msg.msg_.flowRemovedMask_[0]);
    io.mapRequired("flow_removed_mask_slave", msg.msg_.flowRemovedMask_[1]);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YGETASYNCREPLY_H_
