// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YPORTPROPERTY_H_
#define OFP_YAML_YPORTPROPERTY_H_

#include "ofp/portproperty.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::PortPropertyEthernet> {
  static void mapping(IO &io, ofp::PortPropertyEthernet &prop) {
    using namespace ofp;
    
    OFPPortFeaturesFlags curr = prop.curr();
    OFPPortFeaturesFlags advertised = prop.advertised();
    OFPPortFeaturesFlags supported = prop.supported();
    OFPPortFeaturesFlags peer = prop.peer();
    io.mapRequired("curr", curr);
    io.mapRequired("advertised", advertised);
    io.mapRequired("supported", supported);
    io.mapRequired("peer", peer);
    prop.setCurr(curr);
    prop.setAdvertised(advertised);
    prop.setSupported(supported);
    prop.setPeer(peer);

    io.mapRequired("curr_speed", prop.currSpeed_);
    io.mapRequired("max_speed", prop.maxSpeed_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YPORTPROPERTY_H_
