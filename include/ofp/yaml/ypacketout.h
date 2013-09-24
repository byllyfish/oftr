//  ===== ---- ofp/yaml/ypacketout.h -----------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the PacketOut and 
/// PacketOutBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YPACKETOUT_H
#define OFP_YAML_YPACKETOUT_H

#include "ofp/packetout.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::PacketOut> {

    static void mapping(IO &io, ofp::PacketOut &msg)
    {
        using namespace ofp;

        UInt32 bufferID = msg.bufferId();
        UInt32 inPort = msg.inPort();

        io.mapRequired("buffer_id", bufferID);
        io.mapRequired("in_port", inPort);

        ActionRange actions = msg.actions();
        io.mapRequired("actions", actions);

        ByteRange enetFrame = msg.enetFrame();
        io.mapRequired("enet_frame", enetFrame);
    }
};


template <>
struct MappingTraits<ofp::PacketOutBuilder> {

    static void mapping(IO &io, ofp::PacketOutBuilder &msg)
    {
        using namespace ofp;

        io.mapRequired("buffer_id", msg.msg_.bufferId_);
        io.mapRequired("in_port", msg.msg_.inPort_);

        io.mapRequired("actions", msg.actions_);
        io.mapRequired("enet_frame", msg.enetFrame_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YPACKETOUT_H
