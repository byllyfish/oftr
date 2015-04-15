// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YROLEREQUEST_H_
#define OFP_YAML_YROLEREQUEST_H_

#include "ofp/rolerequest.h"

namespace llvm {
namespace yaml {

const char *const kRoleRequestSchema = R"""({Message/RoleRequest}
type: 'ROLE_REQUEST'
msg:
  role: ControllerRole
  generation_id: UInt64
)""";

template <>
struct MappingTraits<ofp::RoleRequest> {
  static void mapping(IO &io, ofp::RoleRequest &msg) {
    ofp::OFPControllerRole role = msg.role();
    io.mapRequired("role", role);
    io.mapRequired("generation_id", msg.generationId_);
  }
};

template <>
struct MappingTraits<ofp::RoleRequestBuilder> {
  static void mapping(IO &io, ofp::RoleRequestBuilder &msg) {
    ofp::OFPControllerRole role;
    io.mapRequired("role", role);
    msg.setRole(role);
    io.mapRequired("generation_id", msg.msg_.generationId_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YROLEREQUEST_H_
