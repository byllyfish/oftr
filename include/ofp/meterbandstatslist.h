#ifndef OFP_METERBANDSTATSLIST_H_
#define OFP_METERBANDSTATSLIST_H_

#include "ofp/protocollist.h"
#include "ofp/meterbandstatsrange.h"

namespace ofp {

class MeterBandStatsList : public ProtocolList<MeterBandStatsRange> {
public:
    void add(const MeterBandStatsBuilder &stats) { buf_.add(&stats, sizeof(stats)); }
};

}  // namespace ofp

#endif // OFP_METERBANDSTATSLIST_H_
