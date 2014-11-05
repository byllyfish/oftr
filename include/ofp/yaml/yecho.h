// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YECHO_H_
#define OFP_YAML_YECHO_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/echorequest.h"
#include "ofp/echoreply.h"

namespace llvm {
namespace yaml {

// type: OFPT_ECHO_REQUEST
// msg:
//   data: <Bytes>    { Required }

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

// type: OFPT_ECHO_REPLY
// msg:
//   data: <Bytes>    { Required }

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
