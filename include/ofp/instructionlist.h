//  ===== ---- ofp/instructionlist.h -----------------------*- C++ -*- =====  //
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
/// \brief Defines the InstructionList class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_INSTRUCTIONLIST_H_
#define OFP_INSTRUCTIONLIST_H_

#include "ofp/protocollist.h"
#include "ofp/instructions.h"
#include "ofp/instructioniterator.h"
#include "ofp/instructionrange.h"
#include "ofp/bytelist.h"

namespace ofp {

class InstructionList : public ProtocolList<InstructionRange> {
using Inherited = ProtocolList<InstructionRange>;
public:
  using Inherited::Inherited;

  template <class Type>
  void add(const Type &instruction);
};

template <class Type>
inline void InstructionList::add(const Type &instruction) {
  static_assert(Type::type().enumType() != 0, "Type is not an instruction?");
  buf_.add(&instruction, sizeof(instruction));
}

template <>
inline void InstructionList::add(const IT_WRITE_ACTIONS &instruction) {
  buf_.add(&instruction, IT_WRITE_ACTIONS::HeaderSize);
  buf_.add(instruction.data(), instruction.size());
}

template <>
inline void InstructionList::add(const IT_APPLY_ACTIONS &instruction) {
  buf_.add(&instruction, IT_APPLY_ACTIONS::HeaderSize);
  buf_.add(instruction.data(), instruction.size());
}

}  // namespace ofp

#endif  // OFP_INSTRUCTIONLIST_H_
