#ifndef OFP_PACKETCOUNTERLIST_H_
#define OFP_PACKETCOUNTERLIST_H_

#include "ofp/protocollist.h"
#include "ofp/packetcounterrange.h"

namespace ofp {

class PacketCounterList : public ProtocolList<PacketCounterRange> {
 public:
  void add(const PacketCounterBuilder &stats) {
    buf_.add(&stats, sizeof(stats));
  }
};

}  // namespace ofp

#endif  // OFP_PACKETCOUNTERLIST_H_
