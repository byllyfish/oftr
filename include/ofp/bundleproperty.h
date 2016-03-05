// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_BUNDLEPROPERTY_H_
#define OFP_BUNDLEPROPERTY_H_

#include "ofp/constants.h"
#include "ofp/experimenterproperty.h"

namespace ofp {

using BundlePropertyExperimenter =
    detail::ExperimenterProperty<OFPBundleProperty, OFPBPT_EXPERIMENTER>;

class BundlePropertyValidator {
 public:
  static bool validateInput(const PropertyRange &range, Validation *context);
};

}  // namespace ofp

#endif  // OFP_BUNDLEPROPERTY_H_
