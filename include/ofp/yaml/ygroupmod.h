//  ===== ---- ofp/ygroupmod.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for GroupMod and
/// GroupModBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YGROUPMOD_H_
#define OFP_YAML_YGROUPMOD_H_

#include "ofp/groupmod.h"
#include "ofp/yaml/ybucket.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_GROUP_MOD
// msg:
//   command: <UInt16>        { Required }
//   type: <UInt8>            { Required }
//   group_id: <UInt32>       { Required }
//   buckets: [ <Bucket> ]    { Required }
//...

template <>
struct MappingTraits<ofp::GroupMod> {

  static void mapping(IO &io, ofp::GroupMod &msg) {
    io.mapRequired("command", msg.command_);
    io.mapRequired("type", msg.groupType_);
    io.mapRequired("group_id", msg.groupId_);

    ofp::BucketRange buckets = msg.buckets();
    io.mapRequired("buckets", buckets);
  }
};

template <>
struct MappingTraits<ofp::GroupModBuilder> {

  static void mapping(IO &io, ofp::GroupModBuilder &msg) {
    io.mapRequired("command", msg.msg_.command_);
    io.mapRequired("type", msg.msg_.groupType_);
    io.mapRequired("group_id", msg.msg_.groupId_);
    io.mapRequired("buckets", msg.buckets_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YGROUPMOD_H_
