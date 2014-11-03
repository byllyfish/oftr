//  ===== ---- ofp/yaml/ysetconfig.h -----------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the SetConfig and
/// SetConfigBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YSETCONFIG_H_
#define OFP_YAML_YSETCONFIG_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/setconfig.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_SET_CONFIG
// msg:
//   flags: <UInt16>            { Required }
//   miss_send_len: <UInt16>    { Required }
//...

template <>
struct MappingTraits<ofp::SetConfig> {
  static void mapping(IO &io, ofp::SetConfig &msg) {
    io.mapRequired("flags", msg.flags_);
    io.mapRequired("miss_send_len", msg.missSendLen_);
  }
};

template <>
struct MappingTraits<ofp::SetConfigBuilder> {
  static void mapping(IO &io, ofp::SetConfigBuilder &msg) {
    io.mapRequired("flags", msg.msg_.flags_);
    io.mapRequired("miss_send_len", msg.msg_.missSendLen_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YSETCONFIG_H_
