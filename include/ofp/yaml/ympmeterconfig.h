// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YMPMETERCONFIG_H_
#define OFP_YAML_YMPMETERCONFIG_H_

#include "ofp/mpmeterconfig.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPMeterConfig> {
  static void mapping(IO &io, ofp::MPMeterConfig &body) {
    io.mapRequired("flags", body.flags_);
    io.mapRequired("meter_id", body.meterId_);

    ofp::MeterBandRange bands = body.bands();
    io.mapRequired("bands", bands);
  }
};

template <>
struct MappingTraits<ofp::MPMeterConfigBuilder> {
  static void mapping(IO &io, ofp::MPMeterConfigBuilder &body) {
    io.mapRequired("flags", body.msg_.flags_);
    io.mapRequired("meter_id", body.msg_.meterId_);
    io.mapRequired("bands", body.bands_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPMETERCONFIG_H_
