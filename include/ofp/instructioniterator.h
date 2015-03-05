// Copyright 2014-present Bill Fisher. All rights reserved.

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

using InstructionIterator = ProtocolIterator<detail::InstructionIteratorItem,
                                             ProtocolIteratorType::Instruction>;

}  // namespace ofp

#endif  // OFP_INSTRUCTIONITERATOR_H_
