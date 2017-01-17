// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPAGGREGATESTATSREQUEST_H_
#define OFP_MPAGGREGATESTATSREQUEST_H_

#include "ofp/mpflowstatsrequest.h"

namespace ofp {

// MPAggregateStatsRequest is identical to MPFlowStatsRequest. Only the reply is
// different.

using MPAggregateStatsRequest = MPFlowStatsRequest;
using MPAggregateStatsRequestBuilder = MPFlowStatsRequestBuilder;

}  // namespace ofp

#endif  // OFP_MPAGGREGATESTATSREQUEST_H_
