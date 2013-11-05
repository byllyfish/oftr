//  ===== ---- ofp/aggregatestatsrequest.h -----------------*- C++ -*- =====  //
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
/// \brief Defines AggregateStatsRequest and AggregateStatsRequestBuilder
/// classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_AGGREGATESTATSREQUEST_H
#define OFP_AGGREGATESTATSREQUEST_H

#include "ofp/flowstatsrequest.h"

namespace ofp { // <namespace ofp>

// AggregateStatsRequest is identical to FlowStatsRequest. Only the reply is
// different.

using AggregateStatsRequest = FlowStatsRequest;
using AggregateStatsRequestBuilder = FlowStatsRequestBuilder;

} // </namespace ofp>

#endif // OFP_AGGREGATESTATSREQUEST_H
