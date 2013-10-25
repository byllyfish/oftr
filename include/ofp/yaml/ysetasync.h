//  ===== ---- ofp/ysetasync.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for SetAsync and 
/// SetAsyncBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YSETASYNC_H
#define OFP_YAML_YSETASYNC_H

#include "ofp/setasync.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::SetAsync> {

    static void mapping(IO &io, ofp::SetAsync &msg)
    {
        io.mapRequired("packet_in_mask_master", msg.packetInMask_[0]);
        io.mapRequired("packet_in_mask_slave", msg.packetInMask_[1]);
        io.mapRequired("port_status_mask_master", msg.portStatusMask_[0]);
        io.mapRequired("port_status_mask_slave", msg.portStatusMask_[1]);
        io.mapRequired("flow_removed_mask_master", msg.flowRemovedMask_[0]);
        io.mapRequired("flow_removed_mask_slave", msg.flowRemovedMask_[1]);
    }
};


template <>
struct MappingTraits<ofp::SetAsyncBuilder> {

    static void mapping(IO &io, ofp::SetAsyncBuilder &msg)
    {
        io.mapRequired("packet_in_mask_master", msg.msg_.packetInMask_[0]);
        io.mapRequired("packet_in_mask_slave", msg.msg_.packetInMask_[1]);
        io.mapRequired("port_status_mask_master", msg.msg_.portStatusMask_[0]);
        io.mapRequired("port_status_mask_slave", msg.msg_.portStatusMask_[1]);
        io.mapRequired("flow_removed_mask_master", msg.msg_.flowRemovedMask_[0]);
        io.mapRequired("flow_removed_mask_slave", msg.msg_.flowRemovedMask_[1]);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YSETASYNC_H
