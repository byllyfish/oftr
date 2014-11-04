// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YPORT_H_
#define OFP_YAML_YPORT_H_

#include "ofp/port.h"
#include "ofp/portlist.h"
#include "ofp/yaml/ysmallcstring.h"

namespace ofp {
namespace detail {

struct PortInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

//---
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
//...

template <>
struct MappingTraits<ofp::Port> {
  static void mapping(IO &io, ofp::Port &msg) {
    io.mapRequired("port_no", msg.portNo_);
    io.mapRequired("hw_addr", msg.hwAddr_);
    io.mapRequired("name", msg.name_);
    io.mapRequired("config", msg.config_);
    io.mapRequired("state", msg.state_);
    io.mapRequired("curr", msg.curr_);
    io.mapRequired("advertised", msg.advertised_);
    io.mapRequired("supported", msg.supported_);
    io.mapRequired("peer", msg.peer_);
    io.mapRequired("curr_speed", msg.currSpeed_);
    io.mapRequired("max_speed", msg.maxSpeed_);
  }
};

template <>
struct MappingTraits<ofp::PortBuilder> {
  static void mapping(IO &io, ofp::PortBuilder &msg) {
    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("hw_addr", msg.msg_.hwAddr_);
    io.mapRequired("name", msg.msg_.name_);
    io.mapRequired("config", msg.msg_.config_);
    io.mapRequired("state", msg.msg_.state_);
    io.mapRequired("curr", msg.msg_.curr_);
    io.mapRequired("advertised", msg.msg_.advertised_);
    io.mapRequired("supported", msg.msg_.supported_);
    io.mapRequired("peer", msg.msg_.peer_);
    io.mapRequired("curr_speed", msg.msg_.currSpeed_);
    io.mapRequired("max_speed", msg.msg_.maxSpeed_);
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
