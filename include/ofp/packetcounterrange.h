// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PACKETCOUNTERRANGE_H_
#define OFP_PACKETCOUNTERRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/packetcounter.h"

namespace ofp {

using PacketCounterIterator =
    ProtocolIterator<PacketCounter, ProtocolIteratorType::PacketCounter>;
using PacketCounterRange = ProtocolRange<PacketCounterIterator>;

}  // namespace ofp

#endif  // OFP_PACKETCOUNTERRANGE_H_
