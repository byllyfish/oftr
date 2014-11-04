// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YMETERMOD_H_
#define OFP_YAML_YMETERMOD_H_

#include "ofp/metermod.h"
#include "ofp/yaml/ymeterbands.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MeterMod> {
  static void mapping(IO &io, ofp::MeterMod &msg) {
    io.mapRequired("command", msg.command_);
    io.mapRequired("flags", msg.flags_);
    io.mapRequired("meter_id", msg.meterId_);

    ofp::MeterBandRange meterBands = msg.meterBands();
    io.mapRequired("bands", meterBands);
  }
};

template <>
struct MappingTraits<ofp::MeterModBuilder> {
  static void mapping(IO &io, ofp::MeterModBuilder &msg) {
    io.mapRequired("command", msg.msg_.command_);
    io.mapRequired("flags", msg.msg_.flags_);
    io.mapRequired("meter_id", msg.msg_.meterId_);
    io.mapRequired("bands", msg.meterBands_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMETERMOD_H_
