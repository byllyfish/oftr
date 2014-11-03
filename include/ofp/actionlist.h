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

#ifndef OFP_ACTIONLIST_H_
#define OFP_ACTIONLIST_H_

#include "ofp/types.h"
#include "ofp/protocollist.h"
#include "ofp/actionrange.h"
#include "ofp/actions.h"

namespace ofp {

class ActionList : public ProtocolList<ActionRange> {
  using Inherited = ProtocolList<ActionRange>;

 public:
  using Inherited::Inherited;

  template <class Type>
  void add(const Type &action) {
    // Make sure it's an action.
    assert(Type::type() != 0);
    buf_.add(&action, sizeof(action));
  }
};

template <>
inline void ActionList::add(const AT_SET_FIELD_CUSTOM &action) {
  ByteRange value = action.value_;
  size_t paddedLen = PadLength(value.size());
  buf_.add(&action, AT_SET_FIELD_CUSTOM::FixedSize);
  buf_.add(value.data(), value.size());
  buf_.addZeros(paddedLen - value.size());
}

template <>
inline void ActionList::add(const AT_UNKNOWN &action) {
  ByteRange value = action.value_;
  size_t paddedLen = PadLength(AT_UNKNOWN::FixedSize + value.size());
  buf_.add(&action, AT_UNKNOWN::FixedSize);
  buf_.add(value.data(), value.size());
  buf_.addZeros(paddedLen - (AT_UNKNOWN::FixedSize + value.size()));
}

template <>
inline void ActionList::add(const AT_EXPERIMENTER &action) {
  ByteRange value = action.value_;
  size_t paddedLen = PadLength(AT_EXPERIMENTER::FixedSize + value.size());
  buf_.add(&action, AT_EXPERIMENTER::FixedSize);
  buf_.add(value.data(), value.size());
  buf_.addZeros(paddedLen - (AT_EXPERIMENTER::FixedSize + value.size()));
}

}  // namespace ofp

#endif  // OFP_ACTIONLIST_H_
