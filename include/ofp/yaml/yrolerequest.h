// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YROLEREQUEST_H_
#define OFP_YAML_YROLEREQUEST_H_

#include "ofp/rolerequest.h"

namespace llvm {
namespace yaml {

// type: OFPT_ROLE_REQUEST
// msg:
//   role: <UInt32>             { Required }
//   generation_id: <UInt64>    { Required }

template <>
struct MappingTraits<ofp::RoleRequest> {
  static void mapping(IO &io, ofp::RoleRequest &msg) {
    io.mapRequired("role", msg.role_);
    io.mapRequired("generation_id", msg.generationId_);
  }
};

template <>
struct MappingTraits<ofp::RoleRequestBuilder> {
  static void mapping(IO &io, ofp::RoleRequestBuilder &msg) {
    io.mapRequired("role", msg.msg_.role_);
    io.mapRequired("generation_id", msg.msg_.generationId_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YROLEREQUEST_H_
