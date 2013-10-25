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
#include "ofp/yaml/yllvm.h"

//---
// type: OFPT_HELLO
// msg:
//   - key: versions
//     type: List<UInt8>
//     required: False
//     notes: List of supported protocol versions. Use values 1, 2, 3, and 4. 
//            Value 0 and values greater than 4 are ignored. Version 4 is used 
//            for specification 1.3, and version 1 is used for spec 1.0. If the
//            versions list is omitted or empty (after filtering out unsupported
//            values), use the version in the header. If the header version is
//            also omitted, the message specifies all supported versions.
//...

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
