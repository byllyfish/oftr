// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YFEATURESREPLY_H_
#define OFP_YAML_YFEATURESREPLY_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/featuresreply.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/yaml/yport.h"

namespace llvm {
namespace yaml {

const char *const kFeaturesReplySchema = R"""({Message/FeaturesReply}
type: 'FEATURES_REPLY'
msg:
  datapath_id: DatapathID
  n_buffers: UInt32
  n_tables: UInt8
  auxiliary_id: UInt8
  capabilities: CapabilitiesFlags
  actions: ActionTypeFlags             # Version 1 only
  ports: [{Port}...]
)""";

template <>
struct MappingTraits<ofp::FeaturesReply> {
  static void mapping(IO &io, ofp::FeaturesReply &msg) {
    using namespace ofp;

    DatapathID dpid = msg.datapathId();
    Hex32 bufferCount = msg.bufferCount();
    Hex8 tableCount = msg.tableCount();
    Hex8 auxiliaryId = msg.auxiliaryId();
    OFPCapabilitiesFlags capabilities = msg.capabilities();

    io.mapRequired("datapath_id", dpid);
    io.mapRequired("n_buffers", bufferCount);
    io.mapRequired("n_tables", tableCount);
    io.mapRequired("auxiliary_id", auxiliaryId);
    io.mapRequired("capabilities", capabilities);

    if (msg.msgHeader()->version() == OFP_VERSION_1) {
      OFPActionTypeFlags actions = msg.actions();
      io.mapRequired("actions", actions);
    }

    PortRange ports = msg.ports();
    io.mapRequired("ports", ports);
  }
};

template <>
struct MappingTraits<ofp::FeaturesReplyBuilder> {
  static void mapping(IO &io, ofp::FeaturesReplyBuilder &msg) {
    using namespace ofp;

    DatapathID dpid;
    UInt32 bufferCount;
    UInt8 tableCount;
    UInt8 auxiliaryId;
    OFPCapabilitiesFlags capabilities = OFPC_NONE;
    OFPActionTypeFlags actions;

    io.mapRequired("datapath_id", dpid);
    io.mapRequired("n_buffers", bufferCount);
    io.mapRequired("n_tables", tableCount);
    io.mapOptional("auxiliary_id", auxiliaryId, UInt8{});
    io.mapRequired("capabilities", capabilities);
    io.mapOptional("actions", actions, OFPActionTypeFlags{});

    msg.setDatapathId(dpid);
    msg.setBufferCount(bufferCount);
    msg.setTableCount(tableCount);
    msg.setAuxiliaryId(auxiliaryId);
    msg.setCapabilities(capabilities);
    msg.setActions(actions);

    PortList ports;
    io.mapRequired("ports", ports);
    msg.setPorts(ports);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YFEATURESREPLY_H_
