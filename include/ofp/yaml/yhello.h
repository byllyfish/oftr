//  ===== ---- ofp/yhello.h --------------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the Hello and HelloBuilder
/// classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YHELLO_H
#define OFP_YAML_YHELLO_H

#include "ofp/hello.h"
#include "ofp/protocolversions.h"

// FIXME - move this macro to yllvm.h ?
LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(ofp::UInt8);

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::Hello> {

    static void mapping(IO &io, ofp::Hello &msg)
    {
        ofp::ProtocolVersions versions = msg.protocolVersions();
        if (!versions.empty()) {
            std::vector<ofp::UInt8> vers = versions.versions();
            io.mapRequired("versions", vers);
        }
    }
};


template <>
struct MappingTraits<ofp::HelloBuilder> {

    static void mapping(IO &io, ofp::HelloBuilder &msg)
    {
        std::vector<ofp::UInt8> versions;
        io.mapRequired("versions", versions);
        msg.setProtocolVersions(ofp::ProtocolVersions{versions});
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YHELLO_H
