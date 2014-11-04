// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YHELLO_H_
#define OFP_YAML_YHELLO_H_

#include "ofp/hello.h"
#include "ofp/protocolversions.h"
#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_HELLO
// msg:                       { Optional; versions defaults to All. }
//   versions: [ <UInt8> ]    { Optional }
//...

template <>
struct MappingTraits<ofp::Hello> {
  static void mapping(IO &io, ofp::Hello &msg) {
    ofp::ProtocolVersions versions = msg.protocolVersions();
    std::vector<ofp::UInt8> vers = versions.versions();
    io.mapRequired("versions", vers);
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
