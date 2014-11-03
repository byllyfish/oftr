#ifndef OFP_YAML_YMPEXPERIMENTER_H_
#define OFP_YAML_YMPEXPERIMENTER_H_

#include "ofp/mpexperimenter.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPExperimenter> {
  static void mapping(IO &io, ofp::MPExperimenter &body) {
    io.mapRequired("experimenter", body.experimenter_);
    io.mapRequired("exp_type", body.expType_);

    ofp::ByteRange data = body.expData();
    io.mapRequired("data", data);
  }
};

template <>
struct MappingTraits<ofp::MPExperimenterBuilder> {
  static void mapping(IO &io, ofp::MPExperimenterBuilder &body) {
    io.mapRequired("experimenter", body.msg_.experimenter_);
    io.mapRequired("exp_type", body.msg_.expType_);
    io.mapRequired("data", body.data_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPEXPERIMENTER_H_
