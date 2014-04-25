#ifndef OFP_INSTRUCTIONIDRANGE_H_
#define OFP_INSTRUCTIONIDRANGE_H_

#include "ofp/protocolrange.h"

namespace ofp {

namespace detail {

class InstructionIDIteratorItem : private NonCopyable {
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(InstructionType)
  };

  InstructionType type() const { return type_; }

  //UInt16 size() const { return len_; }

 private:
  InstructionType type_;
  Big16 len_;
};

}  // namespace detail


using InstructionIDIterator = ProtocolIterator<detail::InstructionIDIteratorItem>;
using InstructionIDRange = ProtocolRange<InstructionIDIterator>;

}  // namespace ofp

#endif // OFP_INSTRUCTIONIDRANGE_H_
