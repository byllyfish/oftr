//  ===== ---- ofp/yportmod.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for PortMod and PortModBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YPORTMOD_H
#define OFP_YAML_YPORTMOD_H

#include "ofp/portmod.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::PortMod> {

    static void mapping(IO &io, ofp::PortMod &msg)
    {
        io.mapRequired("port_no", msg.portNo_);
        io.mapRequired("hw_addr", msg.hwAddr_);
        io.mapRequired("config", msg.config_);
        io.mapRequired("mask", msg.mask_);
        io.mapRequired("advertise", msg.advertise_);
    }
};

template <>
struct MappingTraits<ofp::PortModBuilder> {

    static void mapping(IO &io, ofp::PortModBuilder &msg)
    {
        io.mapRequired("port_no", msg.msg_.portNo_);
        io.mapRequired("hw_addr", msg.msg_.hwAddr_);
        io.mapRequired("config", msg.msg_.config_);
        io.mapRequired("mask", msg.msg_.mask_);
        io.mapRequired("advertise", msg.msg_.advertise_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YPORTMOD_H