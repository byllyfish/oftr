//  ===== ---- ofp/instructionrange.h ----------------------*- C++ -*- =====  //
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
/// \brief Defines the InstructionRange class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_INSTRUCTIONRANGE_H_
#define OFP_INSTRUCTIONRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/instructioniterator.h"
#include "ofp/actionrange.h"

namespace ofp {

class InstructionRange : public ProtocolRange<InstructionIterator> {
  using Inherited = ProtocolRange<InstructionIterator>;

 public:
  using Inherited::Inherited;

  ActionRange outputActions() const;

  bool validateInput(Validation *context) const;
};

}  // namespace ofp

#endif  // OFP_INSTRUCTIONRANGE_H_
