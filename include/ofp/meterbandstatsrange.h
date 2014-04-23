#ifndef OFP_METERBANDSTATSRANGE_H_
#define OFP_METERBANDSTATSRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/meterbandstats.h"

namespace ofp {

using MeterBandStatsIterator = ProtocolIterator<MeterBandStats>;
using MeterBandStatsRange = ProtocolRange<MeterBandStatsIterator>;

}  // namespace ofp

#endif // OFP_METERBANDSTATSRANGE_H_
