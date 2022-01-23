// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YRAWMESSAGE_H_
#define OFP_YAML_YRAWMESSAGE_H_

#include "ofp/rawmessage.h"
#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

const char *const kRawMessageSchema = R"""({Message/RawMessage}
type: _RAW_MESSAGE
msg:
  type: String | Hex8
  data: HexData
)""";

template <>
struct MappingTraits<ofp::RawMessageBuilder> {
  static void mapping(IO &io, ofp::RawMessageBuilder &msg) {
    ofp::OFPType type = ofp::OFPT_UNSUPPORTED;
    io.mapRequired("type", type);
    msg.setType(type);

    io.mapRequired("data", msg.data_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YRAWMESSAGE_H_
