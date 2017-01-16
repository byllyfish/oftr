// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpgroupfeatures.h"
#include "ofp/validation.h"

using namespace ofp;

bool MPGroupFeatures::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();
  if (length != sizeof(MPGroupFeatures)) {
    context->lengthRemainingIsInvalid(BytePtr(this), sizeof(MPGroupFeatures));
    return false;
  }
  return true;
}
