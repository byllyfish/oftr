#include "ofp/instructionrange.h"
#include "ofp/instructions.h"

using ofp::InstructionRange;
using ofp::ActionRange;


/// \brief Retrieve action list from OFPIT_APPLY_ACTIONS instruction.
ActionRange InstructionRange::outputActions() const {
  InstructionIterator iter = begin();
  InstructionIterator iterEnd = end();

  while (iter != iterEnd) {
    if (iter->type() == IT_APPLY_ACTIONS::type()) {
      return ByteRange{iter.data() + IT_APPLY_ACTIONS::HeaderSize,
                       iter.size() - IT_APPLY_ACTIONS::HeaderSize};
    }
    ++iter;
  }

  return ByteRange{};
}
