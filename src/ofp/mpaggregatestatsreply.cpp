// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
