// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ROLESTATUSPROPERTY_H_
#define OFP_ROLESTATUSPROPERTY_H_

#include "ofp/constants.h"
#include "ofp/experimenterproperty.h"

namespace ofp {

class PropertyRange;
class Validation;

using RoleStatusPropertyExperimenter =
    detail::ExperimenterProperty<OFPRoleStatusProperty, OFPRPT_EXPERIMENTER>;

class RoleStatusPropertyValidator {
 public:
  static bool validateInput(const PropertyRange &range, Validation *context);
};

}  // namespace ofp

#endif  // OFP_ROLESTATUSPROPERTY_H_
