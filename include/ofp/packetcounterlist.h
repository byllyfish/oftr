// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PACKETCOUNTERLIST_H_
#define OFP_PACKETCOUNTERLIST_H_

#include "ofp/packetcounterrange.h"
#include "ofp/protocollist.h"

namespace ofp {

class PacketCounterList : public ProtocolList<PacketCounterRange> {
 public:
  void add(const PacketCounterBuilder &stats) {
    buf_.add(&stats, sizeof(stats));
  }
};

}  // namespace ofp

#endif  // OFP_PACKETCOUNTERLIST_H_
