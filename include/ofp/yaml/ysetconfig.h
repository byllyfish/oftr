// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YSETCONFIG_H_
#define OFP_YAML_YSETCONFIG_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/setconfig.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/ycontrollermaxlen.h"

namespace llvm {
namespace yaml {

const char *const kSetConfigSchema = R"""({Message/SetConfig}
type: SET_CONFIG
msg:
  flags: [ConfigFlags]
  miss_send_len: ControllerMaxLen
)""";

template <>
struct MappingTraits<ofp::SetConfig> {
  static void mapping(IO &io, ofp::SetConfig &msg) {
    io.mapRequired("flags", msg.flags_);
    io.mapRequired("miss_send_len", msg.missSendLen_);
  }
};

template <>
struct MappingTraits<ofp::SetConfigBuilder> {
  static void mapping(IO &io, ofp::SetConfigBuilder &msg) {
    io.mapRequired("flags", msg.msg_.flags_);
    io.mapRequired("miss_send_len", msg.msg_.missSendLen_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YSETCONFIG_H_
