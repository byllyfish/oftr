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
    ofp::OFPMeterModCommand command = msg.command();
    ofp::OFPMeterConfigFlags flags = msg.flags();
    io.mapRequired("command",command);
    io.mapRequired("flags", flags);
    io.mapRequired("meter_id", msg.meterId_);

    ofp::MeterBandRange meterBands = msg.meterBands();
    io.mapRequired("bands", meterBands);
  }
};

template <>
struct MappingTraits<ofp::MeterModBuilder> {
  static void mapping(IO &io, ofp::MeterModBuilder &msg) {
    ofp::OFPMeterModCommand command;
    io.mapRequired("command", command);
    msg.setCommand(command);
    ofp::OFPMeterConfigFlags flags;
    io.mapRequired("flags", flags);
    msg.setFlags(flags);
    io.mapRequired("meter_id", msg.msg_.meterId_);
    io.mapRequired("bands", msg.meterBands_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMETERMOD_H_
