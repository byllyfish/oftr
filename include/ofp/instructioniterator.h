// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
    return Interpret_cast<Type>(this);
  }

  // N.B. InstructionRange overrides validateInput.
  bool validateInput(Validation *context) const { return true; }

 private:
  InstructionType type_;
  Big16 len_;
};

}  // namespace detail

using InstructionIterator = ProtocolIterator<detail::InstructionIteratorItem,
                                             ProtocolIteratorType::Instruction>;

}  // namespace ofp

#endif  // OFP_INSTRUCTIONITERATOR_H_
