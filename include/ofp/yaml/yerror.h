// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YERROR_H_
#define OFP_YAML_YERROR_H_

#include "ofp/error.h"
#include "ofp/yaml/ybytelist.h"

namespace llvm {
namespace yaml {

const char *const kErrorSchema = R"""({Message/Error}
type: 'ERROR'
msg:
  type: ErrorType
  code: ErrorCode
  data: HexString
)""";

template <>
struct MappingTraits<ofp::Error> {
  static void mapping(IO &io, ofp::Error &msg) {
    ofp::OFPErrorType type = msg.errorType();
    ofp::OFPErrorCode code = msg.errorCode();
    ofp::ByteRange data = msg.errorData();
    io.mapRequired("type", type);
    io.mapRequired("code", code);
    io.mapRequired("data", data);
  }
};

template <>
struct MappingTraits<ofp::ErrorBuilder> {
  static void mapping(IO &io, ofp::ErrorBuilder &msg) {
    ofp::OFPErrorType type;
    ofp::OFPErrorCode code;
    ofp::ByteList data;
    io.mapRequired("type", type);
    io.mapRequired("code", code);
    io.mapRequired("data", data);

    if (type == ofp::OFPET_EXPERIMENTER || OFPErrorCodeIsUnknown(code)) {
      code = OFPErrorCodeSetType(code, type);
    }

    msg.setErrorCode(code);
    msg.setErrorData(data.data(), data.size());

    // If the type is present, make sure it matches the error code.
    if (type != OFPErrorCodeGetType(code)) {
      io.setError("Error code does not correspond to error type");
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YERROR_H_
