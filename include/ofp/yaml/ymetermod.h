// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YMETERMOD_H_
#define OFP_YAML_YMETERMOD_H_

#include "ofp/metermod.h"
#include "ofp/yaml/ymeterbands.h"

namespace llvm {
namespace yaml {

const char *const kMeterModSchema = R"""({Message/MeterMod}
type: METER_MOD
msg:
  command: MeterModCommand
  flags: [MeterConfigFlags]
  meter_id: MeterNumber
  bands: [MeterBand]
)""";

template <>
struct MappingTraits<ofp::MeterMod> {
  static void mapping(IO &io, ofp::MeterMod &msg) {
    ofp::OFPMeterModCommand command = msg.command();
    ofp::OFPMeterConfigFlags flags = msg.flags();
    io.mapRequired("command", command);
    io.mapRequired("flags", flags);
    io.mapRequired("meter_id", msg.meterId_);

    ofp::MeterBandRange meterBands = msg.meterBands();
    io.mapRequired("bands", meterBands);
  }
};

template <>
struct MappingTraits<ofp::MeterModBuilder> {
  static void mapping(IO &io, ofp::MeterModBuilder &msg) {
    ofp::OFPMeterModCommand command = ofp::OFPMC_ADD;
    io.mapRequired("command", command);
    msg.setCommand(command);
    ofp::OFPMeterConfigFlags flags = ofp::OFPMF_NONE;
    io.mapRequired("flags", flags);
    msg.setFlags(flags);
    io.mapRequired("meter_id", msg.msg_.meterId_);
    io.mapRequired("bands", msg.meterBands_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMETERMOD_H_
