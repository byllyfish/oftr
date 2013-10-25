//  ===== ---- ofp/match.h ---------------------------------*- C++ -*- =====  //
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
/// \brief Defines the Match class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MATCH_H
#define OFP_MATCH_H

#include "ofp/standardmatch.h"

namespace ofp { // <namespace ofp>

class Match {
public:
  explicit Match(OXMRange range) : oxm_{range} {
    log::debug("Match created:", range);
  }

  explicit Match(const deprecated::StandardMatch *match)
      : oxm_{match->toOXMList()} {}

  /// \returns number of items in the match.
  size_t itemCount() const {
    return OXMIterator::distance(oxm_.begin(), oxm_.end());
  }

  OXMIterator begin() const { return oxm_.begin(); }

  OXMIterator end() const { return oxm_.end(); }

  OXMRange toRange() const { return oxm_.toRange(); }

private:
  OXMList oxm_;
};

} // </namespace ofp>

#endif // OFP_MATCH_H
