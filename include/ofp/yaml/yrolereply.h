// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YROLEREPLY_H_
#define OFP_YAML_YROLEREPLY_H_

#include "ofp/rolereply.h"

namespace llvm {
namespace yaml {

const char *const kRoleReplySchema = R"""({Message/RoleReply}
type: ROLE_REPLY
msg:
  role: ControllerRole
  generation_id: UInt64
)""";

template <>
struct MappingTraits<ofp::RoleReply> {
  static void mapping(IO &io, ofp::RoleReply &msg) {
    ofp::OFPControllerRole role = msg.role();
    io.mapRequired("role", role);
    io.mapRequired("generation_id", msg.generationId_);
  }
};

template <>
struct MappingTraits<ofp::RoleReplyBuilder> {
  static void mapping(IO &io, ofp::RoleReplyBuilder &msg) {
    ofp::OFPControllerRole role;
    io.mapRequired("role", role);
    msg.setRole(role);
    io.mapRequired("generation_id", msg.msg_.generationId_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YROLEREPLY_H_
