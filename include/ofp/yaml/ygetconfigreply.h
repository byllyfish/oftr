// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YGETCONFIGREPLY_H_
#define OFP_YAML_YGETCONFIGREPLY_H_

#include "ofp/getconfigreply.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_GET_CONFIG_REPLY
// msg:
//   flags: <UInt16>            { Required }
//   miss_send_len: <UInt16>    { Required }
//...

template <>
struct MappingTraits<ofp::GetConfigReply> {
  static void mapping(IO &io, ofp::GetConfigReply &msg) {
    io.mapRequired("flags", msg.flags_);
    io.mapRequired("miss_send_len", msg.missSendLen_);
  }
};

template <>
struct MappingTraits<ofp::GetConfigReplyBuilder> {
  static void mapping(IO &io, ofp::GetConfigReplyBuilder &msg) {
    io.mapRequired("flags", msg.msg_.flags_);
    io.mapRequired("miss_send_len", msg.msg_.missSendLen_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YGETCONFIGREPLY_H_
