// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_BUNDLEPROPERTY_H_
#define OFP_BUNDLEPROPERTY_H_

#include "ofp/experimenterproperty.h"
#include "ofp/constants.h"

namespace ofp {

using BundlePropertyExperimenter =
    detail::ExperimenterProperty<OFPBundleProperty, OFPBPT_EXPERIMENTER>;

class BundlePropertyValidator {
 public:
  static bool validateInput(const PropertyRange &range, Validation *context);
};

}  // namespace ofp

#endif  // OFP_BUNDLEPROPERTY_H_
