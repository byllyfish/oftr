// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YSETCONFIG_H_
#define OFP_YAML_YSETCONFIG_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/setconfig.h"
#include "ofp/yaml/ycontrollermaxlen.h"

namespace llvm {
namespace yaml {

// type: OFPT_SET_CONFIG
// msg:
//   flags: <UInt16>            { Required }
//   miss_send_len: <UInt16>    { Required }

template <>
struct MappingTraits<ofp::SetConfig> {
  static void mapping(IO &io, ofp::SetConfig &msg) {
    ofp::OFPConfigFlags flags = msg.flags_;
    io.mapRequired("flags", flags);
    io.mapRequired("miss_send_len", msg.missSendLen_);
  }
};

template <>
struct MappingTraits<ofp::SetConfigBuilder> {
  static void mapping(IO &io, ofp::SetConfigBuilder &msg) {
    ofp::OFPConfigFlags flags;
    io.mapRequired("flags", flags);
    msg.setFlags(flags);
    io.mapRequired("miss_send_len", msg.msg_.missSendLen_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YSETCONFIG_H_
