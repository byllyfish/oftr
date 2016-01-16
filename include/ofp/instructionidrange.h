// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
