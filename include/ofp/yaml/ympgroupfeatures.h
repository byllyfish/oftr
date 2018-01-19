// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPGROUPFEATURES_H_
#define OFP_YAML_YMPGROUPFEATURES_H_

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

    auto actions_all = body.actionsALL();
    auto actions_sel = body.actionsSEL();
    auto actions_ind = body.actionsIND();
    auto actions_ff = body.actionsFF();

    io.mapRequired("actions_all", actions_all);
    io.mapRequired("actions_sel", actions_sel);
    io.mapRequired("actions_ind", actions_ind);
    io.mapRequired("actions_ff", actions_ff);
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

    ofp::OFPActionTypeFlags actions_all;
    ofp::OFPActionTypeFlags actions_sel;
    ofp::OFPActionTypeFlags actions_ind;
    ofp::OFPActionTypeFlags actions_ff;

    io.mapRequired("actions_all", actions_all);
    io.mapRequired("actions_sel", actions_sel);
    io.mapRequired("actions_ind", actions_ind);
    io.mapRequired("actions_ff", actions_ff);

    body.setActionsALL(actions_all);
    body.setActionsSEL(actions_sel);
    body.setActionsIND(actions_ind);
    body.setActionsFF(actions_ff);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPGROUPFEATURES_H_
