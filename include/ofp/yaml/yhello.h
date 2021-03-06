// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YHELLO_H_
#define OFP_YAML_YHELLO_H_

#include "ofp/hello.h"
#include "ofp/protocolversions.h"
#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

const char *const kHelloSchema = R"""({Message/Hello}
type: HELLO
msg:
  versions: !optout [UInt8]
)""";

template <>
struct MappingTraits<ofp::Hello> {
  static void mapping(IO &io, ofp::Hello &msg) {
    // Output versions hello element only if present.
    if (msg.msgLength() > ofp::Hello::MinLength) {
      ofp::ProtocolVersions versions = msg.protocolVersions();
      std::vector<ofp::UInt8> vers = versions.versions();
      io.mapRequired("versions", vers);
    }
  }
};

template <>
struct MappingTraits<ofp::HelloBuilder> {
  static void mapping(IO &io, ofp::HelloBuilder &msg) {
    std::vector<ofp::UInt8> versions;
    io.mapOptional("versions", versions);
    msg.setProtocolVersions(ofp::ProtocolVersions::fromVector(versions));
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YHELLO_H_
