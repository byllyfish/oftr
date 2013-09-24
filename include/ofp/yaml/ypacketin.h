//  ===== ---- ofp/yaml/ypacketin.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the PacketIn and 
/// PacketInBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YPACKETIN_H
#define OFP_YAML_YPACKETIN_H

#include "ofp/packetin.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::PacketIn> {

    static void mapping(IO &io, ofp::PacketIn &msg)
    {
        using namespace ofp;

        // Remember that PacketIn uses cross-wired accessors.
        UInt32 bufferID = msg.bufferId();
        UInt16 totalLen = msg.totalLen();

        io.mapRequired("buffer_id", bufferID);
        io.mapRequired("total_len", totalLen);

        UInt32 inPort = msg.inPort();
    	UInt32 inPhyPort = msg.inPhyPort();
    	UInt64 metadata = msg.metadata();
        io.mapRequired("in_port", inPort);
        io.mapRequired("in_phy_port", inPhyPort);
        io.mapRequired("metadata", metadata);

        OFPPacketInReason reason = msg.reason();
        UInt8 tableID = msg.tableID();
        UInt64 cookie = msg.cookie();
        io.mapRequired("reason", reason);
        io.mapRequired("table_id", tableID);
        io.mapRequired("cookie", cookie);

        ofp::ByteRange enetFrame = msg.enetFrame();
        io.mapRequired("enet_frame", enetFrame);
    }
};


template <>
struct MappingTraits<ofp::PacketInBuilder> {

    static void mapping(IO &io, ofp::PacketInBuilder &msg)
    {
        using namespace ofp;

        io.mapRequired("buffer_id", msg.msg_.bufferId_);
        io.mapRequired("total_len", msg.msg_.totalLen_);

        UInt32 inPort;
    	UInt32 inPhyPort;
    	UInt64 metadata;
        io.mapRequired("in_port", inPort);
        io.mapRequired("in_phy_port", inPhyPort);
        io.mapRequired("metadata", metadata);
        msg.setInPort(inPort);
        msg.setInPhyPort(inPhyPort);
        msg.setMetadata(metadata);

        io.mapRequired("reason", msg.msg_.reason_);
        io.mapRequired("table_id", msg.msg_.tableID_);
        io.mapRequired("cookie", msg.msg_.cookie_);

        io.mapRequired("enet_frame", msg.enetFrame_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YPACKETIN_H
