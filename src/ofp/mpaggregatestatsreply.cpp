// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/mpaggregatestatsreply.h"
#include "ofp/validation.h"

using namespace ofp;

bool MPAggregateStatsReply::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();
  if (length != sizeof(MPAggregateStatsReply)) {
    context->lengthRemainingIsInvalid(BytePtr(this),
                                      sizeof(MPAggregateStatsReply));
    return false;
  }
  return true;
}
