// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YGETCONFIGREPLY_H_
#define OFP_YAML_YGETCONFIGREPLY_H_

#include "ofp/getconfigreply.h"

namespace llvm {
namespace yaml {

const char *const kGetConfigReplySchema = R"""({Message/GetConfigReply}
type: 'GET_CONFIG_REPLY'
msg:
  flags: ConfigFlags
  miss_send_len: ControllerMaxLen
)""";

template <>
struct MappingTraits<ofp::GetConfigReply> {
  static void mapping(IO &io, ofp::GetConfigReply &msg) {
    ofp::OFPConfigFlags flags = msg.flags();
    io.mapRequired("flags", flags);
    io.mapRequired("miss_send_len", msg.missSendLen_);
  }
};

template <>
struct MappingTraits<ofp::GetConfigReplyBuilder> {
  static void mapping(IO &io, ofp::GetConfigReplyBuilder &msg) {
    ofp::OFPConfigFlags flags;
    io.mapRequired("flags", flags);
    msg.setFlags(flags);
    io.mapRequired("miss_send_len", msg.msg_.missSendLen_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YGETCONFIGREPLY_H_
