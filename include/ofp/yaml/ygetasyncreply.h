// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YGETASYNCREPLY_H_
#define OFP_YAML_YGETASYNCREPLY_H_

#include "ofp/getasyncreply.h"
#include "ofp/yaml/yasyncconfigproperty.h"

namespace llvm {
namespace yaml {

const char *const kGetAsyncReplySchema = R"""({Message/GetAsyncReply}
type: GET_ASYNC_REPLY
msg:
  packet_in_slave: !optout [PacketInFlags]
  packet_in_master: !optout [PacketInFlags]
  port_status_slave: !optout [PortStatusFlags]
  port_status_master: !optout [PortStatusFlags]
  flow_removed_slave: !optout [FlowRemovedFlags]
  flow_removed_master: !optout [FlowRemovedFlags]
  role_status_slave: !optout [RoleStatusFlags]
  role_status_master: !optout [RoleStatusFlags]
  table_status_slave: !optout [TableStatusFlags]
  table_status_master: !optout [TableStatusFlags]
  request_forward_slave: !optout [RequestForwardFlags]
  request_forward_master: !optout [RequestForwardFlags]
  properties: !opt [ExperimenterProperty]
)""";

template <>
struct MappingTraits<ofp::GetAsyncReply> {
  static void mapping(IO &io, ofp::GetAsyncReply &msg) {
    using namespace ofp;

    PropertyRange props = msg.properties();

    OFPPacketInFlags pktInSlave;
    if (props.value<AsyncConfigPropertyPacketInSlave>(&pktInSlave)) {
      io.mapRequired("packet_in_slave", pktInSlave);
    }

    OFPPacketInFlags pktInMaster;
    if (props.value<AsyncConfigPropertyPacketInMaster>(&pktInMaster)) {
      io.mapRequired("packet_in_master", pktInMaster);
    }

    OFPPortStatusFlags portStatusSlave;
    if (props.value<AsyncConfigPropertyPortStatusSlave>(&portStatusSlave)) {
      io.mapRequired("port_status_slave", portStatusSlave);
    }

    OFPPortStatusFlags portStatusMaster;
    if (props.value<AsyncConfigPropertyPortStatusMaster>(&portStatusMaster)) {
      io.mapRequired("port_status_master", portStatusMaster);
    }

    OFPFlowRemovedFlags flowRemovedSlave;
    if (props.value<AsyncConfigPropertyFlowRemovedSlave>(&flowRemovedSlave)) {
      io.mapRequired("flow_removed_slave", flowRemovedSlave);
    }

    OFPFlowRemovedFlags flowRemovedMaster;
    if (props.value<AsyncConfigPropertyFlowRemovedMaster>(&flowRemovedMaster)) {
      io.mapRequired("flow_removed_master", flowRemovedMaster);
    }

    OFPRoleStatusFlags roleStatusSlave;
    if (props.value<AsyncConfigPropertyRoleStatusSlave>(&roleStatusSlave)) {
      io.mapRequired("role_status_slave", roleStatusSlave);
    }

    OFPRoleStatusFlags roleStatusMaster;
    if (props.value<AsyncConfigPropertyRoleStatusMaster>(&roleStatusMaster)) {
      io.mapRequired("role_status_master", roleStatusMaster);
    }

    OFPTableStatusFlags tableStatusSlave;
    if (props.value<AsyncConfigPropertyTableStatusSlave>(&tableStatusSlave)) {
      io.mapRequired("table_status_slave", tableStatusSlave);
    }

    OFPTableStatusFlags tableStatusMaster;
    if (props.value<AsyncConfigPropertyTableStatusMaster>(&tableStatusMaster)) {
      io.mapRequired("table_status_master", tableStatusMaster);
    }

    OFPRequestForwardFlags requestForwSlave;
    if (props.value<AsyncConfigPropertyRequestForwardSlave>(
            &requestForwSlave)) {
      io.mapRequired("requestforward_slave", requestForwSlave);
    }

    OFPRequestForwardFlags requestForwMaster;
    if (props.value<AsyncConfigPropertyRequestForwardSlave>(
            &requestForwMaster)) {
      io.mapRequired("requestforward_master", requestForwMaster);
    }

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::AsyncConfigPropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::GetAsyncReplyBuilder> {
  static void mapping(IO &io, ofp::GetAsyncReplyBuilder &msg) {
    using namespace ofp;

    PropertyList props;

    Optional<OFPPacketInFlags> pktMaster;
    Optional<OFPPacketInFlags> pktSlave;
    io.mapOptional("packet_in_slave", pktSlave);
    io.mapOptional("packet_in_master", pktMaster);
    if (pktSlave)
      props.add(AsyncConfigPropertyPacketInSlave{*pktSlave});
    if (pktMaster)
      props.add(AsyncConfigPropertyPacketInMaster{*pktMaster});

    Optional<OFPPortStatusFlags> portMaster;
    Optional<OFPPortStatusFlags> portSlave;
    io.mapOptional("port_status_slave", portSlave);
    io.mapOptional("port_status_master", portMaster);
    if (portSlave)
      props.add(AsyncConfigPropertyPortStatusSlave{*portSlave});
    if (portMaster)
      props.add(AsyncConfigPropertyPortStatusMaster{*portMaster});

    Optional<OFPFlowRemovedFlags> flowMaster;
    Optional<OFPFlowRemovedFlags> flowSlave;
    io.mapOptional("flow_removed_slave", flowSlave);
    io.mapOptional("flow_removed_master", flowMaster);
    if (flowSlave)
      props.add(AsyncConfigPropertyFlowRemovedSlave{*flowSlave});
    if (flowMaster)
      props.add(AsyncConfigPropertyFlowRemovedMaster{*flowMaster});

    Optional<OFPRoleStatusFlags> roleMaster;
    Optional<OFPRoleStatusFlags> roleSlave;
    io.mapOptional("role_status_slave", roleSlave);
    io.mapOptional("role_status_master", roleMaster);
    if (roleSlave)
      props.add(AsyncConfigPropertyRoleStatusSlave{*roleSlave});
    if (roleMaster)
      props.add(AsyncConfigPropertyRoleStatusMaster{*roleMaster});

    Optional<OFPTableStatusFlags> tableMaster;
    Optional<OFPTableStatusFlags> tableSlave;
    io.mapOptional("table_status_slave", tableSlave);
    io.mapOptional("table_status_master", tableMaster);
    if (tableSlave)
      props.add(AsyncConfigPropertyTableStatusSlave{*tableSlave});
    if (tableMaster)
      props.add(AsyncConfigPropertyTableStatusMaster{*tableMaster});

    Optional<OFPRequestForwardFlags> forwMaster;
    Optional<OFPRequestForwardFlags> forwSlave;
    io.mapOptional("requestforward_slave", forwSlave);
    io.mapOptional("requestforward_master", forwMaster);
    if (forwSlave)
      props.add(AsyncConfigPropertyRequestForwardSlave{*forwSlave});
    if (forwMaster)
      props.add(AsyncConfigPropertyRequestForwardMaster{*forwMaster});

    io.mapOptional("properties",
                   Ref_cast<ofp::detail::AsyncConfigPropertyList>(props));
    msg.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YGETASYNCREPLY_H_
