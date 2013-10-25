//  ===== ---- ofp/yflowremoved.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for FlowRemoved and 
/// FlowRemovedBuilder.
//  ===== ------------------------------------------------------------ =====  //


#ifndef OFP_YAML_YFLOWREMOVED_H
#define OFP_YAML_YFLOWREMOVED_H

#include "ofp/flowremoved.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::FlowRemoved> {

    static void mapping(IO &io, ofp::FlowRemoved &msg)
    {
        io.mapRequired("cookie", msg.cookie_);
        io.mapRequired("priority", msg.priority_);
        io.mapRequired("reason", msg.reason_);
        io.mapRequired("table_id", msg.tableId_);
        io.mapRequired("duration_sec", msg.durationSec_);
        io.mapRequired("duration_nsec", msg.durationNSec_);
        io.mapRequired("idle_timeout", msg.idleTimeout_);
        io.mapRequired("hard_timeout", msg.hardTimeout_);
        io.mapRequired("packet_count", msg.packetCount_);
        io.mapRequired("byte_count", msg.byteCount_);

        ofp::Match m = msg.match();
        io.mapRequired("match", m);
    }
};

template <>
struct MappingTraits<ofp::FlowRemovedBuilder> {

    static void mapping(IO &io, ofp::FlowRemovedBuilder &msg)
    {
        io.mapRequired("cookie", msg.msg_.cookie_);
        io.mapRequired("priority", msg.msg_.priority_);
        io.mapRequired("reason", msg.msg_.reason_);
        io.mapRequired("table_id", msg.msg_.tableId_);
        io.mapRequired("duration_sec", msg.msg_.durationSec_);
        io.mapRequired("duration_nsec", msg.msg_.durationNSec_);
        io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
        io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
        io.mapRequired("packet_count", msg.msg_.packetCount_);
        io.mapRequired("byte_count", msg.msg_.byteCount_);

        io.mapRequired("match", msg.match_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YFLOWREMOVED_H
