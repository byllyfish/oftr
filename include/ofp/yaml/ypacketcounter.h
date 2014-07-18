#ifndef OFP_YAML_YPACKETCOUNTER_H_
#define OFP_YAML_YPACKETCOUNTER_H_

#include "ofp/packetcounter.h"

namespace ofp {
namespace detail {

struct PacketCounterInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::PacketCounter> {

  static void mapping(IO &io, ofp::PacketCounter &stats) {
    io.mapRequired("packet_count", stats.packetCount_);
    io.mapRequired("byte_count", stats.byteCount_);
  }
};

template <>
struct MappingTraits<ofp::PacketCounterBuilder> {

  static void mapping(IO &io, ofp::PacketCounterBuilder &stats) {
    io.mapRequired("packet_count", stats.msg_.packetCount_);
    io.mapRequired("byte_count", stats.msg_.byteCount_);
  }
};


template <>
struct MappingTraits<ofp::detail::PacketCounterInserter> {
  static void mapping(IO &io, ofp::detail::PacketCounterInserter &inserter) {
    using namespace ofp;

    PacketCounterList &statsList = reinterpret_cast<PacketCounterList &>(inserter);
    PacketCounterBuilder stats;
    MappingTraits<PacketCounterBuilder>::mapping(io, stats);
    statsList.add(stats);
  }
};

template <>
struct SequenceTraits<ofp::PacketCounterRange> {
  using iterator = ofp::PacketCounterIterator;

  static iterator begin(IO &io, ofp::PacketCounterRange &range) {
    return range.begin();
  }

  static iterator end(IO &io, ofp::PacketCounterRange &range) {
    return range.end();
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
  }
};

template <>
struct SequenceTraits<ofp::PacketCounterList> {

  static size_t size(IO &io, ofp::PacketCounterList &list) { return 0; }

  static ofp::detail::PacketCounterInserter &element(IO &io, ofp::PacketCounterList &list,
                                            size_t index) {
    return reinterpret_cast<ofp::detail::PacketCounterInserter &>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YPACKETCOUNTER_H_
