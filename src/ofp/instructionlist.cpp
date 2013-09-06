#include "ofp/instructionlist.h"

namespace ofp { // <namespace ofp>

/// \brief Retrieve action list from OFPIT_APPLY_ACTIONS instruction.
ActionRange InstructionList::toActions() const
{
    InstructionIterator iter = begin();
    InstructionIterator iterEnd = end();

    while (iter != iterEnd) {
        if (iter.type() == IT_APPLY_ACTIONS::type()) {
            return ByteRange{ iter.data() + IT_APPLY_ACTIONS::HeaderSize,
                              iter.size() - IT_APPLY_ACTIONS::HeaderSize };
        }
        ++iter;
    }

    return ByteRange{};
}

} // </namespace ofp>
