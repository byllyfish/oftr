// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YERROR_H_
#define OFP_YAML_YERROR_H_

#include "ofp/error.h"
#include "ofp/yaml/ybytelist.h"

namespace llvm {
namespace yaml {

const char *const kErrorSchema = R"""({Message/Error}
type: ERROR
msg:
  type: ErrorType
  code: ErrorCode
  data: HexData
  _text: !optout String
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

    std::string text = msg.errorText();
    if (!text.empty())
      io.mapRequired("_text", text);
  }
};

template <>
struct MappingTraits<ofp::ErrorBuilder> {
  static void mapping(IO &io, ofp::ErrorBuilder &msg) {
    ofp::OFPErrorType type = ofp::OFPET_UNKNOWN_FLAG;
    ofp::OFPErrorCode code = ofp::OFPEC_UNKNOWN_FLAG;
    ofp::ByteList data;
    io.mapRequired("type", type);
    io.mapRequired("code", code);
    io.mapRequired("data", data);

    std::string ignore;
    io.mapOptional("_text", ignore);

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
