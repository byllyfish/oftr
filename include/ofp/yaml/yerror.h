//  ===== ---- ofp/yaml/yerror.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the Error and ErrorBuilder
/// classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YERROR_H
#define OFP_YAML_YERROR_H

#include "ofp/error.h"
#include "ofp/yaml/ybytelist.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::Error> {

    static void mapping(IO &io, ofp::Error &msg)
    {
        ofp::UInt16 type = msg.errorType();
        ofp::UInt16 code = msg.errorCode();
        ofp::ByteRange data = msg.errorData();
        io.mapRequired("type", type);
        io.mapRequired("code", code);
        io.mapRequired("data", data);
    }
};


template <>
struct MappingTraits<ofp::ErrorBuilder> {

    static void mapping(IO &io, ofp::ErrorBuilder &msg)
    {
        ofp::UInt16 type;
        ofp::UInt16 code;
        ofp::ByteList data;
        io.mapRequired("type", type);
        io.mapRequired("code", code);
        io.mapRequired("data", data);
        msg.setErrorType(type);
        msg.setErrorCode(code);
        msg.setErrorData(data.data(), data.size());
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YERROR_H
