// Copyright 2014-present Bill Fisher. All rights reserved.

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
