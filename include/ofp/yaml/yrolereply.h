// Copyright 2014-present Bill Fisher. All rights reserved.

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
