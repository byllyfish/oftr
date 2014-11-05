// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YERROR_H_
#define OFP_YAML_YERROR_H_

#include "ofp/error.h"
#include "ofp/yaml/ybytelist.h"

namespace llvm {
namespace yaml {

// type: OFPT_ERROR
// msg:
//   type: <UInt16>    { Required }
//   code: <UInt16>    { Required }
//   data: <Bytes>     { Required }

template <>
struct MappingTraits<ofp::Error> {
  static void mapping(IO &io, ofp::Error &msg) {
    Hex16 type = msg.errorType();
    Hex16 code = msg.errorCode();
    ofp::ByteRange data = msg.errorData();
    io.mapRequired("type", type);
    io.mapRequired("code", code);
    io.mapRequired("data", data);
  }
};

template <>
struct MappingTraits<ofp::ErrorBuilder> {
  static void mapping(IO &io, ofp::ErrorBuilder &msg) {
    ofp::UInt16 type;
    ofp::UInt16 code;
    ofp::ByteList data;
    io.mapRequired("type", type);
    io.mapRequired("code", code);
    io.mapRequired("data", data);
    msg.setErrorType(type);
    msg.setErrorCode(code);
    msg.setErrorData(data.data(), data.size());
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YERROR_H_
