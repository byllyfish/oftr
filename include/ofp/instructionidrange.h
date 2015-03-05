// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_INSTRUCTIONIDRANGE_H_
#define OFP_INSTRUCTIONIDRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/instructionid.h"

namespace ofp {

using InstructionIDIterator =
    ProtocolIterator<InstructionID, ProtocolIteratorType::InstructionID>;
using InstructionIDRange = ProtocolRange<InstructionIDIterator>;

}  // namespace ofp

#endif  // OFP_INSTRUCTIONIDRANGE_H_
