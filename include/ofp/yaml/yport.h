// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YPORT_H_
#define OFP_YAML_YPORT_H_

#include "ofp/port.h"
#include "ofp/portlist.h"
#include "ofp/yaml/yportnumber.h"
#include "ofp/yaml/yportproperty.h"
#include "ofp/yaml/ysmallcstring.h"

namespace ofp {
namespace detail {

struct PortInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

// When you update the port schema, make the corresponding changes to
// PortStatus message schema (yportstatus.h).

const char *const kPortSchema = R"""({Struct/Port}
port_no: PortNumber
hw_addr: MacAddress
name: Str16
config: [PortConfigFlags]
state: [PortStateFlags]
curr: [PortFeaturesFlags]
advertised: [PortFeaturesFlags]
supported: [PortFeaturesFlags]
peer: [PortFeaturesFlags]
curr_speed: UInt32
max_speed: UInt32
optical: !optout
  supported: [OpticalPortFeaturesFlags]
  tx_min_freq_lmda: UInt32
  tx_max_freq_lmda: UInt32
  tx_grid_freq_lmda: UInt32
  rx_min_freq_lmda: UInt32
  rx_max_freq_lmda: UInt32
  rx_grid_freq_lmda: UInt32
  tx_pwr_min: UInt16
  tx_pwr_max: UInt16
properties: !opt [ExperimenterProperty]
)""";

template <>
struct MappingTraits<ofp::Port> {
  static void mapping(IO &io, ofp::Port &msg) {
    using namespace ofp;

    io.mapRequired("port_no", msg.portNo_);
    io.mapRequired("hw_addr", msg.hwAddr_);
    io.mapRequired("name", msg.name_);

    OFPPortConfigFlags config = msg.config();
    io.mapRequired("config", config);

    OFPPortStateFlags state = msg.state();
    io.mapRequired("state", state);

    PropertyRange props = msg.properties();

    auto eprop = props.findProperty(PortPropertyEthernet::type());
    if (eprop != props.end()) {
      const PortPropertyEthernet &eth = eprop->property<PortPropertyEthernet>();
      MappingTraits<PortPropertyEthernet>::mapping(io, RemoveConst_cast(eth));
    } else {
      // If property is missing, write out empty values.
      PortPropertyEthernet empty;
      MappingTraits<PortPropertyEthernet>::mapping(io, empty);
    }

    auto oprop = props.findProperty(PortPropertyOptical::type());
    if (oprop != props.end()) {
      const PortPropertyOptical &opt = oprop->property<PortPropertyOptical>();
      io.mapRequired("optical", RemoveConst_cast(opt));
    }

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::PortPropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::PortBuilder> {
  static void mapping(IO &io, ofp::PortBuilder &msg) {
    using namespace ofp;

    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("hw_addr", msg.msg_.hwAddr_);
    io.mapRequired("name", msg.msg_.name_);
    io.mapRequired("config", msg.msg_.config_);
    io.mapRequired("state", msg.msg_.state_);

    PortPropertyEthernet eth;
    MappingTraits<PortPropertyEthernet>::mapping(io, eth);

    Optional<PortPropertyOptical> opt;
    io.mapOptional("optical", opt);

    PropertyList props;
    props.add(eth);
    if (opt) {
      props.add(*opt);
    }

    io.mapOptional("properties",
                   Ref_cast<ofp::detail::PortPropertyList>(props));
    msg.setProperties(props);
  }
};

template <>
struct MappingTraits<ofp::detail::PortInserter> {
  static void mapping(IO &io, ofp::detail::PortInserter &inserter) {
    using namespace ofp;

    PortList &ports = Ref_cast<PortList>(inserter);
    PortBuilder port;
    MappingTraits<PortBuilder>::mapping(io, port);
    ports.add(port);
  }
};

template <>
struct SequenceTraits<ofp::PortRange> {
  using iterator = ofp::PortIterator;

  static iterator begin(IO &io, ofp::PortRange &range) { return range.begin(); }

  static iterator end(IO &io, ofp::PortRange &range) { return range.end(); }

  static void next(iterator &iter, iterator iterEnd) { ++iter; }
};

template <>
struct SequenceTraits<ofp::PortList> {
  static size_t size(IO &io, ofp::PortList &list) { return 0; }

  static ofp::detail::PortInserter &element(IO &io, ofp::PortList &list,
                                            size_t index) {
    return Ref_cast<ofp::detail::PortInserter>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORT_H_
