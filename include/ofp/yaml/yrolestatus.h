// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YROLESTATUS_H_
#define OFP_YAML_YROLESTATUS_H_

#include "ofp/rolestatus.h"
#include "ofp/yaml/yrolestatusproperty.h"

namespace llvm {
namespace yaml {

// type: OFPT_ROLE_STATUS
// msg:
//   role: <OFPControllerRole>  { Required }
//   reason: <UInt8>            { Required }
//   generation_id: <UInt64>    { Required }

template <>
struct MappingTraits<ofp::RoleStatus> {
  static void mapping(IO &io, ofp::RoleStatus &msg) {
    ofp::OFPControllerRole role = msg.role();
    io.mapRequired("role", role);
    io.mapRequired("reason", msg.reason_);
    io.mapRequired("generation_id", msg.generationId_);

    ofp::PropertyRange props = msg.properties();
    io.mapRequired("properties",
                   Ref_cast<ofp::detail::RoleStatusPropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::RoleStatusBuilder> {
  static void mapping(IO &io, ofp::RoleStatusBuilder &msg) {
    ofp::OFPControllerRole role;
    io.mapRequired("role", role);
    msg.setRole(role);
    io.mapRequired("reason", msg.msg_.reason_);
    io.mapRequired("generation_id", msg.msg_.generationId_);

    ofp::PropertyList props;
    io.mapRequired("properties",
                   Ref_cast<ofp::detail::RoleStatusPropertyList>(props));
    msg.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YROLESTATUS_H_
