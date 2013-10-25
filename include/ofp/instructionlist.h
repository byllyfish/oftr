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

#ifndef OFP_INSTRUCTIONLIST_H
#define OFP_INSTRUCTIONLIST_H

#include "ofp/types.h"
#include "ofp/instructions.h"
#include "ofp/instructioniterator.h"
#include "ofp/bytelist.h"

namespace ofp { // <namespace ofp>

class InstructionRange;

class InstructionList {
public:
  InstructionList() = default;
  InstructionList(const InstructionRange &range);

  InstructionIterator begin() const {
    return InstructionIterator{data()};
  }

  InstructionIterator end() const {
    return InstructionIterator{data() + size()};
  }

  const UInt8 *data() const { return buf_.data(); }

  size_t size() const { return buf_.size(); }

  template <class Type>
  void add(const Type &instruction);

  ActionRange toActions() const;

private:
  ByteList buf_;

#if 0
    void add(const void *data, size_t length)
    {
        const UInt8 *ptr = static_cast<const UInt8 *>(data);
        buf_.insert(buf_.end(), ptr, ptr + length);
    }
#endif
};

template <class Type>
inline void InstructionList::add(const Type &instruction) {
  static_assert(Type::type().type() != 0, "Type is not an instruction?");
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

} // </namespace ofp>

#endif // OFP_INSTRUCTIONLIST_H
