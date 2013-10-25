//  ===== ---- ofp/ygetconfigreply.h -----------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for GetConfigReply and 
/// GetConfigReplyBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YGETCONFIGREPLY_H
#define OFP_YAML_YGETCONFIGREPLY_H

#include "ofp/getconfigreply.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::GetConfigReply> {

    static void mapping(IO &io, ofp::GetConfigReply &msg)
    {
        io.mapRequired("flags", msg.flags_);
        io.mapRequired("miss_send_len", msg.missSendLen_);
    }
};

template <>
struct MappingTraits<ofp::GetConfigReplyBuilder> {

    static void mapping(IO &io, ofp::GetConfigReplyBuilder &msg)
    {
        io.mapRequired("flags", msg.msg_.flags_);
        io.mapRequired("miss_send_len", msg.msg_.missSendLen_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YGETCONFIGREPLY_H
