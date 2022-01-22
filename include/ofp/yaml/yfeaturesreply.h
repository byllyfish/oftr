// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YFEATURESREPLY_H_
#define OFP_YAML_YFEATURESREPLY_H_

#include "ofp/featuresreply.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yport.h"

namespace llvm {
namespace yaml {

const char *const kFeaturesReplySchema = R"""({Message/FeaturesReply}
type: FEATURES_REPLY
msg:
  datapath_id: DatapathID
  n_buffers: UInt32
  n_tables: UInt8
  auxiliary_id: !opt UInt8
  capabilities: [CapabilitiesFlags]
  actions: !optout [ActionTypeFlags]  # version=1
  ports: !opt [Port]
)""";

template <>
struct MappingTraits<ofp::FeaturesReply> {
  static void mapping(IO &io, ofp::FeaturesReply &msg) {
    using namespace ofp;

    io.mapRequired("datapath_id", msg.datapathId_);
    io.mapRequired("n_buffers", msg.bufferCount_);
    io.mapRequired("n_tables", msg.tableCount_);
    io.mapRequired("auxiliary_id", msg.auxiliaryId_);

    OFPCapabilitiesFlags capabilities = msg.capabilities();
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

    io.mapRequired("datapath_id", msg.msg_.datapathId_);
    io.mapRequired("n_buffers", msg.msg_.bufferCount_);
    io.mapRequired("n_tables", msg.msg_.tableCount_);
    io.mapOptional("auxiliary_id", msg.msg_.auxiliaryId_);
    io.mapRequired("capabilities", msg.msg_.capabilities_);

    OFPActionTypeFlags actions = OFPATF_NONE;
    io.mapOptional("actions", actions, OFPActionTypeFlags{});
    msg.setActions(actions);

    PortList ports;
    io.mapOptional("ports", ports);
    msg.setPorts(ports);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YFEATURESREPLY_H_
