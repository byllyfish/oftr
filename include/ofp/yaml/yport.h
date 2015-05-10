// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YPORT_H_
#define OFP_YAML_YPORT_H_

#include "ofp/port.h"
#include "ofp/portlist.h"
#include "ofp/yaml/ysmallcstring.h"
#include "ofp/yaml/yportproperty.h"

namespace ofp {
namespace detail {

struct PortInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

// <Port> :=
//   port_no: <UInt32>         { Required }
//   hw_addr: <EnetAddress>    { Required }
//   name: <Str15>             { Required }
//   config: <UInt32>          { Required }
//   state: <UInt32>           { Required }
//   curr: <UInt32>            { Required }
//   advertised: <UInt32>      { Required }
//   supported: <UInt32>       { Required }
//   peer: <UInt32>            { Required }
//   curr_speed: <UInt32>      { Required }
//   max_speed: <UInt32>       { Required }

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

    auto prop = props.findProperty(PortPropertyEthernet::type());
    if (prop != props.end()) {
      const PortPropertyEthernet &eth = prop->property<PortPropertyEthernet>();
      io.mapRequired("ethernet", RemoveConst_cast(eth));
    }

    #if 0
    OFPPortFeaturesFlags curr = msg.curr();
    OFPPortFeaturesFlags advertised = msg.advertised();
    OFPPortFeaturesFlags supported = msg.supported();
    OFPPortFeaturesFlags peer = msg.peer();
    io.mapRequired("curr", curr);
    io.mapRequired("advertised", advertised);
    io.mapRequired("supported", supported);
    io.mapRequired("peer", peer);

    io.mapRequired("curr_speed", msg.currSpeed_);
    io.mapRequired("max_speed", msg.maxSpeed_);
    #endif //0
  }
};

template <>
struct MappingTraits<ofp::PortBuilder> {
  static void mapping(IO &io, ofp::PortBuilder &msg) {
    using namespace ofp;

    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("hw_addr", msg.msg_.hwAddr_);
    io.mapRequired("name", msg.msg_.name_);

    OFPPortConfigFlags config;
    io.mapRequired("config", config);
    msg.setConfig(config);

    OFPPortStateFlags state;
    io.mapRequired("state", state);
    msg.setState(state);

    PortPropertyEthernet eth;
    io.mapRequired("ethernet", eth);   // FIXME(bfish) - make optional...

    PropertyList props;
    props.add(eth);
    msg.setProperties(props);

    #if 0
    OFPPortFeaturesFlags curr;
    OFPPortFeaturesFlags advertised;
    OFPPortFeaturesFlags supported;
    OFPPortFeaturesFlags peer;
    io.mapRequired("curr", curr);
    io.mapRequired("advertised", advertised);
    io.mapRequired("supported", supported);
    io.mapRequired("peer", peer);
    msg.setCurr(curr);
    msg.setAdvertised(advertised);
    msg.setSupported(supported);
    msg.setPeer(peer);

    io.mapRequired("curr_speed", msg.msg_.currSpeed_);
    io.mapRequired("max_speed", msg.msg_.maxSpeed_);
    #endif //0
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
