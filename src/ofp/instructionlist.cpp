//  ===== ---- ofp/instructionlist.cpp ---------------------*- C++ -*- =====  //
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
/// \brief Implements InstructionList class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/instructionlist.h"
#include "ofp/instructionrange.h"

using namespace ofp;

InstructionList::InstructionList(const InstructionRange &range) {
  buf_.add(range.data(), range.size());
}

/// \brief Retrieve action list from OFPIT_APPLY_ACTIONS instruction.
ActionRange InstructionList::toActions() const {
  InstructionIterator iter = begin();
  InstructionIterator iterEnd = end();

  while (iter != iterEnd) {
    if (iter.type() == IT_APPLY_ACTIONS::type()) {
      return ByteRange{iter.data() + IT_APPLY_ACTIONS::HeaderSize,
                       iter.size() - IT_APPLY_ACTIONS::HeaderSize};
    }
    ++iter;
  }

  return ByteRange{};
}
