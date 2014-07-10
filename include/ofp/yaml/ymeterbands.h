#ifndef OFP_YAML_YMETERBANDS_H_
#define OFP_YAML_YMETERBANDS_H_

#include "ofp/meterbands.h"

namespace ofp {
namespace detail {

struct MeterBandInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MeterBandIterator::Element> {
  static void mapping(IO &io, ofp::MeterBandIterator::Element &elem) {
    using namespace ofp;

    OFPMeterBandType meterType = elem.type();
    io.mapRequired("type", meterType);

    switch (meterType) {
      case MeterBandDrop::type() : {
        auto p = elem.meterBand<MeterBandDrop>();
        UInt32 rate = p.rate();
        UInt32 burstSize = p.burstSize();
        io.mapRequired("rate", rate);
        io.mapRequired("burst_size", burstSize);
        break;
      }
      case MeterBandDscpRemark::type() : {
        auto p = elem.meterBand<MeterBandDscpRemark>();
        UInt32 rate = p.rate();
        UInt32 burstSize = p.burstSize();
        UInt8 precLevel = p.precLevel();
        io.mapRequired("rate", rate);
        io.mapRequired("burst_size", burstSize);
        io.mapRequired("prec_level", precLevel);
        break;
      }
      case MeterBandExperimenter::type() : {
        auto p = elem.meterBand<MeterBandExperimenter>();
        UInt32 rate = p.rate();
        UInt32 burstSize = p.burstSize();
        UInt32 experimenter = p.experimenter();
        io.mapRequired("rate", rate);
        io.mapRequired("burst_size", burstSize);
        io.mapRequired("experimenter", experimenter);
        break;
      }
      default:
        break;
    }
  }
};

template <>
struct MappingTraits<ofp::detail::MeterBandInserter> {
  static void mapping(IO &io, ofp::detail::MeterBandInserter &inserter) {
    using namespace ofp;

    MeterBandList &list = reinterpret_cast<MeterBandList &>(inserter);

    OFPMeterBandType meterType;
    io.mapRequired("type", meterType);

    switch (meterType) {
      case MeterBandDrop::type() : {
        UInt32 rate, burstSize;
        io.mapRequired("rate", rate);
        io.mapRequired("burst_size", burstSize);
        list.add(MeterBandDrop{rate, burstSize});
        break;
      }
      case MeterBandDscpRemark::type() : {
        UInt32 rate, burstSize;
        UInt8 precLevel;
        io.mapRequired("rate", rate);
        io.mapRequired("burst_size", burstSize);
        io.mapRequired("prec_level", precLevel);
        list.add(MeterBandDscpRemark{rate, burstSize, precLevel});
        break;
      }
      case MeterBandExperimenter::type() : {
        UInt32 rate, burstSize, experimenter;
        io.mapRequired("rate", rate);
        io.mapRequired("burst_size", burstSize);
        io.mapRequired("experimenter", experimenter);
        list.add(MeterBandExperimenter{rate, burstSize, experimenter});
        break;
      }
      default:
        break;
    }
  }
};


template <>
struct SequenceTraits<ofp::MeterBandRange> {
  using iterator = ofp::MeterBandIterator;

  static iterator begin(IO &io, ofp::MeterBandRange &range) {
    return range.begin();
  }

  static iterator end(IO &io, ofp::MeterBandRange &range) {
    return range.end();
  }
};

template <>
struct SequenceTraits<ofp::MeterBandList> {
  static size_t size(IO &io, ofp::MeterBandList &list) { return 0; }

  static ofp::detail::MeterBandInserter &element(IO &io,
                                                 ofp::MeterBandList &list,
                                                 size_t index) {
    return reinterpret_cast<ofp::detail::MeterBandInserter &>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMETERBANDS_H_
