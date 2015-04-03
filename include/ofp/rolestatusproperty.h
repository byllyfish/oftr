// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_ROLESTATUSPROPERTY_H_
#define OFP_ROLESTATUSPROPERTY_H_

#include "ofp/experimenterproperty.h"
#include "ofp/constants.h"

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
