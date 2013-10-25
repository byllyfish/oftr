//  ===== ---- ofp/actionlist.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the ActionList class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ACTIONLIST_H
#define OFP_ACTIONLIST_H

#include "ofp/types.h"
#include "ofp/bytelist.h"
#include "ofp/actionrange.h"

namespace ofp { // <namespace ofp>

class ActionList {
public:
  ActionList() = default;

  const UInt8 *data() const { return buf_.data(); }

  size_t size() const { return buf_.size(); }

  template <class Type>
  void add(const Type &action) {
    // Make sure it's an action.
    assert(Type::type() != 0);
    buf_.add(&action, sizeof(action));
  }

  ActionRange toRange() const {
    return ActionRange{buf_.toRange()};
  }

  void operator=(const ActionRange &range) { buf_ = range.toByteRange(); }

private:
  ByteList buf_;
};

} // </namespace ofp>

#endif // OFP_ACTIONLIST_H
