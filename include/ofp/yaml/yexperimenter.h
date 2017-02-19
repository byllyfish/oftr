// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YEXPERIMENTER_H_
#define OFP_YAML_YEXPERIMENTER_H_

#include "ofp/experimenter.h"
#include "ofp/yaml/ybytelist.h"

namespace llvm {
namespace yaml {

// type: OFPT_EXPERIMENTER
// msg:
//   experimenter: <UInt32>      { Required }
//   exp_type: <UInt32>          { Required }
//   data: <Bytes>  { Required }

template <>
struct MappingTraits<ofp::Experimenter> {
  static void mapping(IO &io, ofp::Experimenter &msg) {
    Hex32 experimenter = msg.experimenter();
    Hex32 expType = msg.expType();
    ofp::ByteRange data = msg.expData();
    io.mapRequired("experimenter", experimenter);
    io.mapRequired("exp_type", expType);
    io.mapRequired("data", data);
  }
};

template <>
struct MappingTraits<ofp::ExperimenterBuilder> {
  static void mapping(IO &io, ofp::ExperimenterBuilder &msg) {
    ofp::UInt32 experimenter;
    ofp::UInt32 expType;
    ofp::ByteList data;
    io.mapRequired("experimenter", experimenter);
    io.mapRequired("exp_type", expType);
    io.mapRequired("data", data);
    msg.setExperimenter(experimenter);
    msg.setExpType(expType);
    msg.setExpData(data.data(), data.size());
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YEXPERIMENTER_H_
