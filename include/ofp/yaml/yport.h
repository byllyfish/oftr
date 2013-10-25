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

#ifndef OFP_YAML_YPORT_H
#define OFP_YAML_YPORT_H

#include "ofp/port.h"
#include "ofp/yaml/ysmallcstring.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::Port> {

    static void mapping(IO &io, ofp::Port &msg)
    {
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

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YPORT_H
