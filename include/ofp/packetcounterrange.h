// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PACKETCOUNTERRANGE_H_
#define OFP_PACKETCOUNTERRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/packetcounter.h"

namespace ofp {

using PacketCounterIterator = ProtocolIterator<PacketCounter, ProtocolIteratorType::PacketCounter>;
using PacketCounterRange = ProtocolRange<PacketCounterIterator>;

}  // namespace ofp

#endif  // OFP_PACKETCOUNTERRANGE_H_
