// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PROTOCOLITERATORTYPE_H_
#define OFP_PROTOCOLITERATORTYPE_H_

#include "ofp/types.h"

namespace ofp {

enum class ProtocolIteratorType {
  Unspecified,
  ActionID,
  Action,
  Bucket,
  HelloElement,
  InstructionID,
  Instruction,
  MeterBand,
  OXMID,
  // OXM itself is implemented by OXMIterator, not a ProtocolIterator.
  PacketCounter,
  Port,
  Property,
  Queue,
  QueueV1,
};

}  // namespace ofp

#endif  // OFP_PROTOCOLITERATORTYPE_H_
