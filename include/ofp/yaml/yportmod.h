// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YPORTMOD_H_
#define OFP_YAML_YPORTMOD_H_

#include "ofp/portmod.h"

namespace llvm {
namespace yaml {

const char *const kPortModSchema = R"""({Message/PortMod}
type: 'PORT_MOD'
msg:
  port_no: PortNumber
  hw_addr: EnetAddress
  config: PortConfigFlags
  mask: PortConfigFlags
  advertise: PortFeaturesFlags
)""";

template <>
struct MappingTraits<ofp::PortMod> {
  static void mapping(IO &io, ofp::PortMod &msg) {
    io.mapRequired("port_no", msg.portNo_);
    io.mapRequired("hw_addr", msg.hwAddr_);

    ofp::OFPPortConfigFlags config = msg.config();
    ofp::OFPPortConfigFlags mask = msg.mask();
    io.mapRequired("config", config);
    io.mapRequired("mask", mask);

    ofp::OFPPortFeaturesFlags advertise = msg.advertise();
    io.mapRequired("advertise", advertise);
  }
};

template <>
struct MappingTraits<ofp::PortModBuilder> {
  static void mapping(IO &io, ofp::PortModBuilder &msg) {
    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("hw_addr", msg.msg_.hwAddr_);

    ofp::OFPPortConfigFlags config = ofp::OFPPC_NONE;
    ofp::OFPPortConfigFlags mask = ofp::OFPPC_NONE;
    io.mapRequired("config", config);
    io.mapRequired("mask", mask);
    msg.setConfig(config);
    msg.setMask(mask);

    ofp::OFPPortFeaturesFlags advertise;
    io.mapRequired("advertise", advertise);
    msg.setAdvertise(advertise);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORTMOD_H_
