// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YGETASYNCREPLY_H_
#define OFP_YAML_YGETASYNCREPLY_H_

#include "ofp/getasyncreply.h"

namespace llvm {
namespace yaml {

// type: OFPT_GET_ASYNC_REPLY
// msg:
//   packet_in_mask_master: <UInt32>       { Required }
//   packet_in_mask_slave: <UInt32>        { Required }
//   port_status_mask_master: <UInt32>     { Required }
//   port_status_mask_slave: <UInt32>      { Required }
//   flow_removed_mask_master: <UInt32>    { Required }
//   flow_removed_mask_slave: <UInt32>     { Required }

template <>
struct MappingTraits<ofp::GetAsyncReply> {
  static void mapping(IO &io, ofp::GetAsyncReply &msg) {
    using namespace ofp;

    OFPPacketInFlags pktMaster = msg.masterPacketInMask();
    OFPPacketInFlags pktSlave = msg.slavePacketInMask();
    io.mapRequired("packet_in_mask_master", pktMaster);
    io.mapRequired("packet_in_mask_slave", pktSlave);

    OFPPortStatusFlags portMaster = msg.masterPortStatusMask();
    OFPPortStatusFlags portSlave = msg.slavePortStatusMask();
    io.mapRequired("port_status_mask_master", portMaster);
    io.mapRequired("port_status_mask_slave", portSlave);

    OFPFlowRemovedFlags flowMaster = msg.masterFlowRemovedMask();
    OFPFlowRemovedFlags flowSlave = msg.slaveFlowRemovedMask();
    io.mapRequired("flow_removed_mask_master", flowMaster);
    io.mapRequired("flow_removed_mask_slave", flowSlave);
  }
};

template <>
struct MappingTraits<ofp::GetAsyncReplyBuilder> {
  static void mapping(IO &io, ofp::GetAsyncReplyBuilder &msg) {
    using namespace ofp;

    OFPPacketInFlags pktMaster;
    OFPPacketInFlags pktSlave;
    io.mapRequired("packet_in_mask_master", pktMaster);
    io.mapRequired("packet_in_mask_slave", pktSlave);
    msg.setMasterPacketInMask(pktMaster);
    msg.setSlavePacketInMask(pktSlave);

    OFPPortStatusFlags portMaster = OFPPRF_NONE;
    OFPPortStatusFlags portSlave = OFPPRF_NONE;
    io.mapRequired("port_status_mask_master", portMaster);
    io.mapRequired("port_status_mask_slave", portSlave);
    msg.setMasterPortStatusMask(portMaster);
    msg.setSlavePortStatusMask(portSlave);

    OFPFlowRemovedFlags flowMaster;
    OFPFlowRemovedFlags flowSlave;
    io.mapRequired("flow_removed_mask_master", flowMaster);
    io.mapRequired("flow_removed_mask_slave", flowSlave);
    msg.setMasterFlowRemovedMask(flowMaster);
    msg.setSlaveFlowRemovedMask(flowSlave);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YGETASYNCREPLY_H_
