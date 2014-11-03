#ifndef OFP_INSTRUCTIONIDRANGE_H_
#define OFP_INSTRUCTIONIDRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/instructionid.h"

namespace ofp {

using InstructionIDIterator = ProtocolIterator<InstructionID>;
using InstructionIDRange = ProtocolRange<InstructionIDIterator>;

}  // namespace ofp

#endif  // OFP_INSTRUCTIONIDRANGE_H_
