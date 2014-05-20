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



bool InstructionRange::validateInput(const char *context) const {
  if (!Inherited::validateInput(context))
    return false;

  for (auto &item : *this) {
    switch (item.type()) {
      case IT_WRITE_ACTIONS::type(): {
        const IT_WRITE_ACTIONS *ins = item.instruction<IT_WRITE_ACTIONS>();
        if (!ins->validateInput(context)) 
          return false;
        break;
      }
      case IT_APPLY_ACTIONS::type(): {
        const IT_APPLY_ACTIONS *ins = item.instruction<IT_APPLY_ACTIONS>();
        if (!ins->validateInput(context)) 
          return false;
        break;
      }
      default:
        break;
    }
  }

  return true;
}
