// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YMPMETERCONFIGREQUEST_H_
#define OFP_YAML_YMPMETERCONFIGREQUEST_H_

#include "ofp/mpmeterconfigrequest.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPMeterConfigRequest> {
  static void mapping(IO &io, ofp::MPMeterConfigRequest &body) {
    io.mapRequired("meter_id", body.meterId_);
  }
};

template <>
struct MappingTraits<ofp::MPMeterConfigRequestBuilder> {
  static void mapping(IO &io, ofp::MPMeterConfigRequestBuilder &body) {
    io.mapRequired("meter_id", body.msg_.meterId_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPMETERCONFIGREQUEST_H_
