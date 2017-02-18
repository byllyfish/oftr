// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YPORTMOD_H_
#define OFP_YAML_YPORTMOD_H_

#include "ofp/portmod.h"
#include "ofp/yaml/yportmodproperty.h"

namespace llvm {
namespace yaml {

const char *const kPortModSchema = R"""({Message/PortMod}
type: PORT_MOD
msg:
  port_no: PortNumber
  hw_addr: MacAddress
  config: [PortConfigFlags]
  mask: [PortConfigFlags]
  ethernet: !opt
    advertise: [PortFeaturesFlags]           # default=[]
  optical: !optout                           # default=null
    configure: [OpticalPortFeaturesFlags]
    freq_lmda: UInt32
    fl_offset: SInt32
    grid_span: UInt32
    tx_pwr: UInt32
  properties: !opt [ExperimenterProperty]    # default=[]
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

    ofp::PropertyRange props = msg.properties();

    auto eprop = props.findProperty(ofp::PortModPropertyEthernet::type());
    if (eprop != props.end()) {
      const ofp::PortModPropertyEthernet &eth =
          eprop->property<ofp::PortModPropertyEthernet>();
      io.mapRequired("ethernet", RemoveConst_cast(eth));
    }

    auto oprop = props.findProperty(ofp::PortModPropertyOptical::type());
    if (oprop != props.end()) {
      const ofp::PortModPropertyOptical &opt =
          oprop->property<ofp::PortModPropertyOptical>();
      io.mapRequired("optical", RemoveConst_cast(opt));
    }

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::PortModPropertyRange>(props));
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

    ofp::PortModPropertyEthernet eth;
    io.mapOptional("ethernet", eth);

    Optional<ofp::PortModPropertyOptical> opt;
    io.mapOptional("optical", opt);

    ofp::PropertyList props;
    props.add(eth);
    if (opt) {
      props.add(*opt);
    }

    io.mapOptional("properties",
                   Ref_cast<ofp::detail::PortModPropertyList>(props));
    msg.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORTMOD_H_
