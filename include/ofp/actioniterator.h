//  ===== ---- ofp/actioniterator.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the ActionIterator class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ACTIONITERATOR_H_
#define OFP_ACTIONITERATOR_H_

#include "ofp/protocoliterator.h"
#include "ofp/actiontype.h"
#include "ofp/oxmrange.h"

namespace ofp {
namespace detail {

class ActionIteratorItem : private NonCopyable {
 public:
  enum { ProtocolIteratorSizeOffset = 2, ProtocolIteratorAlignment = 8 };

  ActionType type() const { return type_; }
  UInt16 size() const { return type_.length(); }

  template <class Type>
  const Type *action() const {
    return reinterpret_cast<const Type *>(this);
  }

  OXMRange oxmRange() const { return value(); }
  ByteRange value() const {
    return {BytePtr(this) + sizeof(ActionType), size() - sizeof(ActionType)};
  }

 private:
  ActionType type_;
};

}  // namespace detail

using ActionIterator = ProtocolIterator<detail::ActionIteratorItem>;

}  // namespace ofp

#endif  // OFP_ACTIONITERATOR_H_
