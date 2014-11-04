// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/mpmeterfeatures.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPMeterFeatures> {
  static void mapping(IO &io, ofp::MPMeterFeatures &body) {
    io.mapRequired("max_meter", body.maxMeter_);
    io.mapRequired("band_types", body.bandTypes_);
    io.mapRequired("capabilities", body.capabilities_);
    io.mapRequired("max_bands", body.maxBands_);
    io.mapRequired("max_color", body.maxColor_);
  }
};

template <>
struct MappingTraits<ofp::MPMeterFeaturesBuilder> {
  static void mapping(IO &io, ofp::MPMeterFeaturesBuilder &body) {
    io.mapRequired("max_meter", body.msg_.maxMeter_);
    io.mapRequired("band_types", body.msg_.bandTypes_);
    io.mapRequired("capabilities", body.msg_.capabilities_);
    io.mapRequired("max_bands", body.msg_.maxBands_);
    io.mapRequired("max_color", body.msg_.maxColor_);
  }
};

}  // namespace yaml
}  // namespace llvm
