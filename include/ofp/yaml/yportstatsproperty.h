// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YPORTSTATSPROPERTY_H_
#define OFP_YAML_YPORTSTATSPROPERTY_H_

#include "ofp/portstatsproperty.h"

namespace ofp {
namespace detail {

struct PortStatsPropertyItem {};
using PortStatsPropertyIterator = TypedPropertyIterator<PortStatsPropertyItem>;
struct PortStatsPropertyRange {};
struct PortStatsPropertyInserter {};
struct PortStatsPropertyList {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::PortStatsPropertyEthernet> {
  static void mapping(IO &io, ofp::PortStatsPropertyEthernet &prop) {
    using namespace ofp;

    io.mapRequired("rx_frame_err", prop.rxFrameErr_);
    io.mapRequired("rx_over_err", prop.rxOverErr_);
    io.mapRequired("rx_crc_err", prop.rxCrcErr_);
    io.mapRequired("collisions", prop.collisions_);
  }
};

template <>
struct MappingTraits<ofp::PortStatsPropertyOptical> {
  static void mapping(IO &io, ofp::PortStatsPropertyOptical &prop) {
    using namespace ofp;

    io.mapRequired("flags", prop.flags_);
    io.mapRequired("tx_freq_lmda", prop.txFreqLmda_);
    io.mapRequired("tx_offset", prop.txOffset_);
    io.mapRequired("tx_grid_span", prop.txGridSpan_);
    io.mapRequired("rx_freq_lmda", prop.rxFreqLmda_);
    io.mapRequired("rx_offset", prop.rxOffset_);
    io.mapRequired("rx_grid_span", prop.rxGridSpan_);
    io.mapRequired("tx_pwr", prop.txPwr_);
    io.mapRequired("rx_pwr", prop.rxPwr_);
    io.mapRequired("bias_current", prop.biasCurrent_);
    io.mapRequired("temperature", prop.temperature_);
  }
};

template <>
struct MappingTraits<ofp::detail::PortStatsPropertyItem> {
  static void mapping(IO &io, ofp::detail::PortStatsPropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    OFPExperimenterPropertyType property =
        static_cast<OFPExperimenterPropertyType>(elem.type());
    io.mapRequired("property", property);

    switch (property) {
      case OFP_EXPERIMENTER_PROPERTY_TYPE: {
        const PortStatsPropertyExperimenter &p =
            elem.property<PortStatsPropertyExperimenter>();

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log_debug("Unsupported PortStatsPropertyItem");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::PortStatsPropertyInserter> {
  static void mapping(IO &io, ofp::detail::PortStatsPropertyInserter &list) {
    using namespace ofp;

    PropertyList &props = Ref_cast<PropertyList>(list);

    OFPExperimenterPropertyType property = OFP_EXPERIMENTER_PROPERTY_TYPE;
    io.mapRequired("property", property);

    if (property == OFP_EXPERIMENTER_PROPERTY_TYPE) {
      UInt32 experimenter;
      io.mapRequired("experimenter", experimenter);

      UInt32 expType;
      io.mapRequired("exp_type", expType);

      ByteList expData;
      io.mapRequired("data", expData);

      props.add(PortStatsPropertyExperimenter{experimenter, expType, expData});

    } else {
      ByteList propData;
      io.mapRequired("data", propData);
      props.add(UnrecognizedProperty{property, propData});
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::PortStatsPropertyRange> {
  using iterator = ofp::detail::PortStatsPropertyIterator;

  static iterator begin(IO &io, ofp::detail::PortStatsPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    auto it = ofp::detail::PortStatsPropertyIterator{props.begin()};
    skip(it, end(io, range));
    return it;
  }

  static iterator end(IO &io, ofp::detail::PortStatsPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::PortStatsPropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
    skip(iter, iterEnd);
  }

  static void skip(iterator &iter, iterator iterEnd) {
    for (; iter < iterEnd; ++iter) {
      ofp::UInt16 type = iter->type();
      if (type >= ofp::OFPPSPT_UNUSED_MIN)
        break;
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::PortStatsPropertyList> {
  static size_t size(IO &io, ofp::detail::PortStatsPropertyList &props) {
    return 0;
  }

  static ofp::detail::PortStatsPropertyInserter &element(
      IO &io, ofp::detail::PortStatsPropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::PortStatsPropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORTSTATSPROPERTY_H_
