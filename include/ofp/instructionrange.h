// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_INSTRUCTIONRANGE_H_
#define OFP_INSTRUCTIONRANGE_H_

#include "ofp/actionrange.h"
#include "ofp/instructioniterator.h"
#include "ofp/protocolrange.h"

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
