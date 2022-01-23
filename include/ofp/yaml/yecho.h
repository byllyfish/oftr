// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YECHO_H_
#define OFP_YAML_YECHO_H_

#include "ofp/echoreply.h"
#include "ofp/echorequest.h"
#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

const char *const kEchoRequestSchema = R"""({Message/EchoRequest}
type: ECHO_REQUEST
msg:
  data: HexData
)""";

template <>
struct MappingTraits<ofp::EchoRequest> {
  static void mapping(IO &io, ofp::EchoRequest &msg) {
    ofp::ByteRange data = msg.echoData();
    io.mapRequired("data", data);
  }
};

template <>
struct MappingTraits<ofp::EchoRequestBuilder> {
  static void mapping(IO &io, ofp::EchoRequestBuilder &msg) {
    ofp::ByteList data;
    io.mapRequired("data", data);
    msg.setEchoData(data.data(), data.size());
  }
};

const char *const kEchoReplySchema = R"""({Message/EchoReply}
type: ECHO_REPLY
msg:
  data: HexData
)""";

template <>
struct MappingTraits<ofp::EchoReply> {
  static void mapping(IO &io, ofp::EchoReply &msg) {
    ofp::ByteRange data = msg.echoData();
    io.mapRequired("data", data);
  }
};

template <>
struct MappingTraits<ofp::EchoReplyBuilder> {
  static void mapping(IO &io, ofp::EchoReplyBuilder &msg) {
    ofp::ByteList data;
    io.mapRequired("data", data);
    msg.setEchoData(data.data(), data.size());
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YECHO_H_
