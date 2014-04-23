#ifndef OFP_YAML_YGROUPFEATURES_H_
#define OFP_YAML_YGROUPFEATURES_H_

#include "ofp/mpgroupfeatures.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPGroupFeatures> {

  static void mapping(IO &io, ofp::MPGroupFeatures &body) {
    io.mapRequired("types", body.types_);
    io.mapRequired("capabilities", body.capabilities_);

    io.mapRequired("max_groups_all", body.maxGroups_[0]);
    io.mapRequired("max_groups_sel", body.maxGroups_[1]);
    io.mapRequired("max_groups_ind", body.maxGroups_[2]);
    io.mapRequired("max_groups_ff", body.maxGroups_[3]);

    io.mapRequired("actions_all", body.actions_[0]);
    io.mapRequired("actions_sel", body.actions_[1]);
    io.mapRequired("actions_ind", body.actions_[2]);
    io.mapRequired("actions_ff", body.actions_[3]);
  }
};

template <>
struct MappingTraits<ofp::MPGroupFeaturesBuilder> {

  static void mapping(IO &io, ofp::MPGroupFeaturesBuilder &body) {
    io.mapRequired("types", body.msg_.types_);
    io.mapRequired("capabilities", body.msg_.capabilities_);

    io.mapRequired("max_groups_all", body.msg_.maxGroups_[0]);
    io.mapRequired("max_groups_sel", body.msg_.maxGroups_[1]);
    io.mapRequired("max_groups_ind", body.msg_.maxGroups_[2]);
    io.mapRequired("max_groups_ff", body.msg_.maxGroups_[3]);

    io.mapRequired("actions_all", body.msg_.actions_[0]);
    io.mapRequired("actions_sel", body.msg_.actions_[1]);
    io.mapRequired("actions_ind", body.msg_.actions_[2]);
    io.mapRequired("actions_ff", body.msg_.actions_[3]);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YGROUPFEATURES_H_
