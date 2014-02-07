//  ===== ---- ofp/yport.h ---------------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the llvm::yaml::MappingTraits for Port.
//  ===== ------------------------------------------------------------ =====  //

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

    PortList &ports = reinterpret_cast<PortList &>(inserter);
    PortBuilder port;
    MappingTraits<PortBuilder>::mapping(io, port);
    ports.add(port);
  }
};

template <>
struct SequenceTraits<ofp::PortRange> {

  static size_t size(IO &io, ofp::PortRange &ports) {
    return ports.itemCount();
  }

  static ofp::PortIterator::Item &element(IO &io, ofp::PortRange &ports,
                                          size_t index) {
    ofp::log::debug("port yaml item", index);
    // FIXME
    ofp::PortIterator iter = ports.begin();
    for (size_t i = 0; i < index; ++i) ++iter;
    return RemoveConst_cast(*iter);
  }
};

template <>
struct SequenceTraits<ofp::PortList> {

  static size_t size(IO &io, ofp::PortList &list) { return 0; }

  static ofp::detail::PortInserter &element(IO &io, ofp::PortList &list,
                                            size_t index) {
    return reinterpret_cast<ofp::detail::PortInserter &>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORT_H_
