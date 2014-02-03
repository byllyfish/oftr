//  ===== ---- ofp/mpaggregatestatsrequest.h ---------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines MPAggregateStatsRequest and MPAggregateStatsRequestBuilder
/// classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MPAGGREGATESTATSREQUEST_H
#define OFP_MPAGGREGATESTATSREQUEST_H

#include "ofp/mpflowstatsrequest.h"

namespace ofp { // <namespace ofp>

// MPAggregateStatsRequest is identical to MPFlowStatsRequest. Only the reply is
// different.

using MPAggregateStatsRequest = MPFlowStatsRequest;
using MPAggregateStatsRequestBuilder = MPFlowStatsRequestBuilder;

} // </namespace ofp>

#endif // OFP_MPAGGREGATESTATSREQUEST_H
