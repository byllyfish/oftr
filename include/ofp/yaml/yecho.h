//  ===== ---- ofp/yaml/yecho.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for EchoRequest, 
/// EchoRequestBuilder, EchoReply, and EchoReplyBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YECHO_H_
#define OFP_YAML_YECHO_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/echorequest.h"
#include "ofp/echoreply.h"

namespace llvm {
namespace yaml {


template <>
struct MappingTraits<ofp::EchoRequest> {

    static void mapping(IO &io, ofp::EchoRequest &msg)
    {
        ofp::ByteRange data = msg.echoData();
        io.mapRequired("data", data);
    }
};


template <>
struct MappingTraits<ofp::EchoRequestBuilder> {

    static void mapping(IO &io, ofp::EchoRequestBuilder &msg)
    {
        ofp::ByteList data;
        io.mapRequired("data", data);
        msg.setEchoData(data.data(), data.size());
    }
};


template <>
struct MappingTraits<ofp::EchoReply> {

    static void mapping(IO &io, ofp::EchoReply &msg)
    {
        ofp::ByteRange data = msg.echoData();
        io.mapRequired("data", data);
    }
};

template <>
struct MappingTraits<ofp::EchoReplyBuilder> {

    static void mapping(IO &io, ofp::EchoReplyBuilder &msg)
    {
        ofp::ByteList data;
        io.mapRequired("data", data);
        msg.setEchoData(data.data(), data.size());
    }
};


}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YECHO_H_
