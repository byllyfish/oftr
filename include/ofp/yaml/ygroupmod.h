// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YGROUPMOD_H_
#define OFP_YAML_YGROUPMOD_H_

#include "ofp/groupmod.h"
#include "ofp/yaml/ybucket.h"

namespace llvm {
namespace yaml {

const char *const kGroupModSchema = R"""({Message/GroupMod}
type: 'GROUP_MOD'
msg:
  command: UInt16
  type: UInt8
  group_id: UInt32
  buckets: [{Bucket}...]
)""";

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
