// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/mpdesc.h"
#include "ofp/validation.h"

using namespace ofp;

bool MPDesc::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();
  if (length != sizeof(MPDesc)) {
    context->lengthRemainingIsInvalid(BytePtr(this), sizeof(MPDesc));
    return false;
  }

  return true;
}
