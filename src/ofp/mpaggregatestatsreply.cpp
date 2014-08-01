//  ===== ---- ofp/mpaggregatestatsreply.cpp ---------------*- C++ -*- =====  //
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
/// \brief Defines the MPAggregateStatsReply class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/mpaggregatestatsreply.h"
#include "ofp/validation.h"

using namespace ofp;

bool MPAggregateStatsReply::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();
  if (length != sizeof(MPAggregateStatsReply)) {
    context->lengthRemainingIsInvalid(BytePtr(this), sizeof(MPAggregateStatsReply));
    return false;
  }
  return true;
}
