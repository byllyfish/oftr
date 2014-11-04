// Copyright 2014-present Bill Fisher. All rights reserved.

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
