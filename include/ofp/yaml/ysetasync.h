// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YSETASYNC_H_
#define OFP_YAML_YSETASYNC_H_

#include "ofp/setasync.h"
#include "ofp/yaml/yasyncconfigproperty.h"

namespace llvm {
namespace yaml {

const char *const kSetAsyncSchema = R"""({Message/SetAsync}
type: SET_ASYNC
msg:
  packet_in_master: [PacketInFlags]
  packet_in_slave: [PacketInFlags]
  port_status_master: [PortStatusFlags]
  port_status_slave: [PortStatusFlags]
  flow_removed_master: [FlowRemovedFlags]
  flow_removed_slave: [FlowRemovedFlags]
)""";

template <>
struct MappingTraits<ofp::SetAsync> {
  static void mapping(IO &io, ofp::SetAsync &msg) {
    using namespace ofp;

    PropertyRange props = msg.properties();

    OFPPacketInFlags pktInMaster;
    if (props.value<AsyncConfigPropertyPacketInMaster>(&pktInMaster)) {
      io.mapRequired("packet_in_master", pktInMaster);
    }

    OFPPacketInFlags pktInSlave;
    if (props.value<AsyncConfigPropertyPacketInSlave>(&pktInSlave)) {
      io.mapRequired("packet_in_slave", pktInSlave);
    }

    OFPPortStatusFlags portStatusMaster;
    if (props.value<AsyncConfigPropertyPortStatusMaster>(&portStatusMaster)) {
      io.mapRequired("port_status_master", portStatusMaster);
    }

    OFPPortStatusFlags portStatusSlave;
    if (props.value<AsyncConfigPropertyPortStatusSlave>(&portStatusSlave)) {
      io.mapRequired("port_status_slave", portStatusSlave);
    }

    OFPFlowRemovedFlags flowRemovedMaster;
    if (props.value<AsyncConfigPropertyFlowRemovedMaster>(&flowRemovedMaster)) {
      io.mapRequired("flow_removed_master", flowRemovedMaster);
    }

    OFPFlowRemovedFlags flowRemovedSlave;
    if (props.value<AsyncConfigPropertyFlowRemovedSlave>(&flowRemovedSlave)) {
      io.mapRequired("flow_removed_slave", flowRemovedSlave);
    }

    OFPRoleStatusFlags roleStatusMaster;
    if (props.value<AsyncConfigPropertyRoleStatusMaster>(&roleStatusMaster)) {
      io.mapRequired("role_status_master", roleStatusMaster);
    }

    OFPRoleStatusFlags roleStatusSlave;
    if (props.value<AsyncConfigPropertyRoleStatusSlave>(&roleStatusSlave)) {
      io.mapRequired("role_status_slave", roleStatusSlave);
    }

    OFPTableStatusFlags tableStatusMaster;
    if (props.value<AsyncConfigPropertyTableStatusMaster>(&tableStatusMaster)) {
      io.mapRequired("table_status_master", tableStatusMaster);
    }

    OFPTableStatusFlags tableStatusSlave;
    if (props.value<AsyncConfigPropertyTableStatusSlave>(&tableStatusSlave)) {
      io.mapRequired("table_status_slave", tableStatusSlave);
    }

    OFPRequestForwardFlags requestForwMaster;
    if (props.value<AsyncConfigPropertyRequestForwardSlave>(
            &requestForwMaster)) {
      io.mapRequired("requestforward_master", requestForwMaster);
    }

    OFPRequestForwardFlags requestForwSlave;
    if (props.value<AsyncConfigPropertyRequestForwardSlave>(
            &requestForwSlave)) {
      io.mapRequired("requestforward_slave", requestForwSlave);
    }

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::AsyncConfigPropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::SetAsyncBuilder> {
  static void mapping(IO &io, ofp::SetAsyncBuilder &msg) {
    using namespace ofp;

    PropertyList props;

    Optional<OFPPacketInFlags> pktMaster;
    Optional<OFPPacketInFlags> pktSlave;
    io.mapOptional("packet_in_master", pktMaster);
    io.mapOptional("packet_in_slave", pktSlave);
    if (pktSlave)
      props.add(AsyncConfigPropertyPacketInSlave{*pktSlave});
    if (pktMaster)
      props.add(AsyncConfigPropertyPacketInMaster{*pktMaster});

    Optional<OFPPortStatusFlags> portMaster;
    Optional<OFPPortStatusFlags> portSlave;
    io.mapOptional("port_status_master", portMaster);
    io.mapOptional("port_status_slave", portSlave);
    if (portSlave)
      props.add(AsyncConfigPropertyPortStatusSlave{*portSlave});
    if (portMaster)
      props.add(AsyncConfigPropertyPortStatusMaster{*portMaster});

    Optional<OFPFlowRemovedFlags> flowMaster;
    Optional<OFPFlowRemovedFlags> flowSlave;
    io.mapOptional("flow_removed_master", flowMaster);
    io.mapOptional("flow_removed_slave", flowSlave);
    if (flowSlave)
      props.add(AsyncConfigPropertyFlowRemovedSlave{*flowSlave});
    if (flowMaster)
      props.add(AsyncConfigPropertyFlowRemovedMaster{*flowMaster});

    Optional<OFPRoleStatusFlags> roleMaster;
    Optional<OFPRoleStatusFlags> roleSlave;
    io.mapOptional("role_status_master", roleMaster);
    io.mapOptional("role_status_slave", roleSlave);
    if (roleSlave)
      props.add(AsyncConfigPropertyRoleStatusSlave{*roleSlave});
    if (roleMaster)
      props.add(AsyncConfigPropertyRoleStatusMaster{*roleMaster});

    Optional<OFPTableStatusFlags> tableMaster;
    Optional<OFPTableStatusFlags> tableSlave;
    io.mapOptional("table_status_master", tableMaster);
    io.mapOptional("table_status_slave", tableSlave);
    if (tableSlave)
      props.add(AsyncConfigPropertyTableStatusSlave{*tableSlave});
    if (tableMaster)
      props.add(AsyncConfigPropertyTableStatusMaster{*tableMaster});

    Optional<OFPRequestForwardFlags> forwMaster;
    Optional<OFPRequestForwardFlags> forwSlave;
    io.mapOptional("requestforward_master", forwMaster);
    io.mapOptional("requestforward_slave", forwSlave);
    if (forwSlave)
      props.add(AsyncConfigPropertyRequestForwardSlave{*forwSlave});
    if (forwMaster)
      props.add(AsyncConfigPropertyRequestForwardMaster{*forwMaster});

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::AsyncConfigPropertyList>(props));
    msg.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YSETASYNC_H_
