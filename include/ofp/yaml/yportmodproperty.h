// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YPORTMODPROPERTY_H_
#define OFP_YAML_YPORTMODPROPERTY_H_

#include "ofp/portmodproperty.h"

namespace ofp {
namespace detail {

struct PortModPropertyItem {};
using PortModPropertyIterator = TypedPropertyIterator<PortModPropertyItem>;
struct PortModPropertyRange {};
struct PortModPropertyInserter {};
struct PortModPropertyList {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::PortModPropertyEthernet> {
  static void mapping(IO &io, ofp::PortModPropertyEthernet &prop) {
    using namespace ofp;
    
    OFPPortFeaturesFlags advertise = prop.advertise();
    io.mapRequired("advertise", advertise);
    prop.setAdvertise(advertise);
  }
};

template <>
struct MappingTraits<ofp::PortModPropertyOptical> {
  static void mapping(IO &io, ofp::PortModPropertyOptical &prop) {
    using namespace ofp;
    
    OFPOpticalPortFeaturesFlags configure = prop.configure();
    io.mapRequired("configure", configure);
    prop.setConfigure(configure);

    io.mapRequired("freq_lmda", prop.freqLmda_);
    io.mapRequired("fl_offset", prop.flOffset_);
    io.mapRequired("grid_span", prop.gridSpan_);
    io.mapRequired("tx_pwr", prop.txPwr_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YPORTMODPROPERTY_H_
