//  ===== ---- ofp/instructioniterator.h -------------------*- C++ -*- =====  //
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
/// \brief Defines the InstructionIterator class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_INSTRUCTIONITERATOR_H_
#define OFP_INSTRUCTIONITERATOR_H_

#include "ofp/protocoliterator.h"
#include "ofp/instructiontype.h"

namespace ofp {

namespace detail {

class InstructionIteratorItem : private NonCopyable {
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(InstructionType),
    ProtocolIteratorAlignment = 8
  };

  InstructionType type() const { return type_; }

  UInt16 size() const { return len_; }

  template <class Type>
  const Type *instruction() const {
    return reinterpret_cast<const Type *>(this);
  }

 private:
  InstructionType type_;
  Big16 len_;
};

}  // namespace detail

using InstructionIterator = ProtocolIterator<detail::InstructionIteratorItem>;

}  // namespace ofp

#endif  // OFP_INSTRUCTIONITERATOR_H_
