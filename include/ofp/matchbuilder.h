//  ===== ---- ofp/matchbuilder.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the MatchBuilder class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MATCHBUILDER_H
#define OFP_MATCHBUILDER_H

#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"
#include "ofp/prerequisites.h"
#include "ofp/constants.h"
#include "ofp/match.h"

namespace ofp { // <namespace ofp>

class MatchBuilder {
public:
  MatchBuilder() = default;
  MatchBuilder(const Match &match) : list_{match.toRange()} {}

  const UInt8 *data() const { return list_.data(); }
  size_t size() const { return list_.size(); }

  /// \returns number of items in the match.
  size_t itemCount() const {
    return OXMIterator::distance(list_.begin(), list_.end());
  }

  OXMIterator begin() const { return list_.begin(); }
  OXMIterator end() const { return list_.end(); }

  template <class ValueType>
  void add(ValueType value) {
    Prerequisites::insertAll(&list_, ValueType::prerequisites());
    if (!Prerequisites::substitute(&list_, ValueType::type(), &value,
                                   sizeof(value))) {
      list_.add(value);
    }
  }

  template <class ValueType>
  void add(ValueType value, ValueType mask) {
    assert(ValueType::maskSupported());
    Prerequisites::insertAll(&list_, ValueType::prerequisites());
    if (!Prerequisites::substitute(&list_, ValueType::type(), &value, &mask,
                                   sizeof(value))) {
      list_.add(value, mask);
    }
  }

  void clear() { list_.clear(); }

  bool validate() const {
    return Prerequisites::checkAll(list_.toRange()) &&
           !Prerequisites::duplicateFieldsDetected(list_.toRange());
  }

  OXMRange toRange() const { return list_.toRange(); }

private:
  OXMList list_;
};

} // </namespace ofp>

#endif // OFP_MATCHBUILDER_H
