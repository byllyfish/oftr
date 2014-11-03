//  ===== ---- ofp/yrolereply.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for RoleReply and
/// RoleReplyBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YROLEREPLY_H_
#define OFP_YAML_YROLEREPLY_H_

#include "ofp/rolereply.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_ROLE_REPLY
// msg:
//   role: <UInt32>             { Required }
//   generation_id: <UInt64>    { Required }
//...

template <>
struct MappingTraits<ofp::RoleReply> {
  static void mapping(IO &io, ofp::RoleReply &msg) {
    io.mapRequired("role", msg.role_);
    io.mapRequired("generation_id", msg.generationId_);
  }
};

template <>
struct MappingTraits<ofp::RoleReplyBuilder> {
  static void mapping(IO &io, ofp::RoleReplyBuilder &msg) {
    io.mapRequired("role", msg.msg_.role_);
    io.mapRequired("generation_id", msg.msg_.generationId_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YROLEREPLY_H_
