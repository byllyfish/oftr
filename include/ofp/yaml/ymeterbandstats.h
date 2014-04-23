#ifndef OFP_YAML_YMETERBANDSTATS_H_
#define OFP_YAML_YMETERBANDSTATS_H_

#include "ofp/meterbandstats.h"

namespace ofp {
namespace detail {

struct MeterBandStatsInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MeterBandStats> {

  static void mapping(IO &io, ofp::MeterBandStats &stats) {
    io.mapRequired("packet_band_count", stats.packetBandCount_);
    io.mapRequired("byte_band_count", stats.byteBandCount_);
  }
};

template <>
struct MappingTraits<ofp::MeterBandStatsBuilder> {

  static void mapping(IO &io, ofp::MeterBandStatsBuilder &stats) {
    io.mapRequired("packet_band_count", stats.msg_.packetBandCount_);
    io.mapRequired("byte_band_count", stats.msg_.byteBandCount_);
  }
};


template <>
struct MappingTraits<ofp::detail::MeterBandStatsInserter> {
  static void mapping(IO &io, ofp::detail::MeterBandStatsInserter &inserter) {
    using namespace ofp;

    MeterBandStatsList &statsList = reinterpret_cast<MeterBandStatsList &>(inserter);
    MeterBandStatsBuilder stats;
    MappingTraits<MeterBandStatsBuilder>::mapping(io, stats);
    statsList.add(stats);
  }
};

template <>
struct SequenceTraits<ofp::MeterBandStatsRange> {

  static size_t size(IO &io, ofp::MeterBandStatsRange &statsList) {
    return statsList.itemCount();
  }

  static ofp::MeterBandStatsIterator::Element &element(IO &io, ofp::MeterBandStatsRange &statsList,
                                          size_t index) {
    ofp::log::debug("queue yaml item", index);
    // FIXME
    ofp::MeterBandStatsIterator iter = statsList.begin();
    for (size_t i = 0; i < index; ++i) ++iter;
    return RemoveConst_cast(*iter);
  }
};

template <>
struct SequenceTraits<ofp::MeterBandStatsList> {

  static size_t size(IO &io, ofp::MeterBandStatsList &list) { return 0; }

  static ofp::detail::MeterBandStatsInserter &element(IO &io, ofp::MeterBandStatsList &list,
                                            size_t index) {
    return reinterpret_cast<ofp::detail::MeterBandStatsInserter &>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YMETERBANDSTATS_H_
