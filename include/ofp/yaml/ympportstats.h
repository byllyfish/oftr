// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPPORTSTATS_H_
#define OFP_YAML_YMPPORTSTATS_H_

#include "ofp/mpportstats.h"
#include "ofp/yaml/yportstatsproperty.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPPortStats> {
  static void mapping(IO &io, ofp::MPPortStats &body) {
    io.mapRequired("port_no", body.portNo_);
    io.mapRequired("duration", body.duration_);
    io.mapRequired("rx_packets", body.rxPackets_);
    io.mapRequired("tx_packets", body.txPackets_);
    io.mapRequired("rx_bytes", body.rxBytes_);
    io.mapRequired("tx_bytes", body.txBytes_);
    io.mapRequired("rx_dropped", body.rxDropped_);
    io.mapRequired("tx_dropped", body.txDropped_);
    io.mapRequired("rx_errors", body.rxErrors_);
    io.mapRequired("tx_errors", body.txErrors_);

    ofp::PropertyRange props = body.properties();

    auto eprop = props.findProperty(ofp::PortStatsPropertyEthernet::type());
    if (eprop != props.end()) {
      const ofp::PortStatsPropertyEthernet &eth =
          eprop->property<ofp::PortStatsPropertyEthernet>();
      io.mapRequired("ethernet", RemoveConst_cast(eth));
    }

    auto oprop = props.findProperty(ofp::PortStatsPropertyOptical::type());
    if (oprop != props.end()) {
      const ofp::PortStatsPropertyOptical &opt =
          oprop->property<ofp::PortStatsPropertyOptical>();
      io.mapRequired("optical", RemoveConst_cast(opt));
    }

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::PortStatsPropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::MPPortStatsBuilder> {
  static void mapping(IO &io, ofp::MPPortStatsBuilder &msg) {
    using namespace ofp;

    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("duration", msg.msg_.duration_);
    io.mapRequired("rx_packets", msg.msg_.rxPackets_);
    io.mapRequired("tx_packets", msg.msg_.txPackets_);
    io.mapRequired("rx_bytes", msg.msg_.rxBytes_);
    io.mapRequired("tx_bytes", msg.msg_.txBytes_);
    io.mapRequired("rx_dropped", msg.msg_.rxDropped_);
    io.mapRequired("tx_dropped", msg.msg_.txDropped_);
    io.mapRequired("rx_errors", msg.msg_.rxErrors_);
    io.mapRequired("tx_errors", msg.msg_.txErrors_);

    PortStatsPropertyEthernet eth;
    io.mapRequired("ethernet", eth);  // FIXME(bfish) - make optional...

    Optional<PortStatsPropertyOptical> opt;
    io.mapOptional("optical", opt);

    PropertyList props;
    props.add(eth);
    if (opt) {
      props.add(*opt);
    }

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::PortStatsPropertyList>(props));
    msg.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPPORTSTATS_H_
