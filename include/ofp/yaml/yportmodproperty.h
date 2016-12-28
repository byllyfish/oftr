// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YPORTMODPROPERTY_H_
#define OFP_YAML_YPORTMODPROPERTY_H_

#include "ofp/portmodproperty.h"

namespace ofp {
namespace detail {

struct PortModPropertyItem {};
using PortModPropertyIterator = TypedPropertyIterator<PortModPropertyItem>;
struct PortModPropertyRange {};
struct PortModPropertyInserter {};
struct PortModPropertyList : public OptionalPropertyList {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::PortModPropertyEthernet> {
  static void mapping(IO &io, ofp::PortModPropertyEthernet &prop) {
    using namespace ofp;

    OFPPortFeaturesFlags advertise = prop.advertise();
    io.mapRequired("advertise", advertise);
    prop.setAdvertise(advertise);
  }
};

template <>
struct MappingTraits<ofp::PortModPropertyOptical> {
  static void mapping(IO &io, ofp::PortModPropertyOptical &prop) {
    using namespace ofp;

    OFPOpticalPortFeaturesFlags configure = prop.configure();
    io.mapRequired("configure", configure);
    prop.setConfigure(configure);

    io.mapRequired("freq_lmda", prop.freqLmda_);
    io.mapRequired("fl_offset", prop.flOffset_);
    io.mapRequired("grid_span", prop.gridSpan_);
    io.mapRequired("tx_pwr", prop.txPwr_);
  }
};

template <>
struct MappingTraits<ofp::detail::PortModPropertyItem> {
  static void mapping(IO &io, ofp::detail::PortModPropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    OFPExperimenterPropertyType property =
        static_cast<OFPExperimenterPropertyType>(elem.type());
    io.mapRequired("property", property);

    switch (property) {
      case OFP_EXPERIMENTER_PROPERTY_TYPE: {
        const PortModPropertyExperimenter &p =
            elem.property<PortModPropertyExperimenter>();

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log_debug("Unsupported PortMod Property");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::PortModPropertyInserter> {
  static void mapping(IO &io, ofp::detail::PortModPropertyInserter &list) {
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

      props.add(PortModPropertyExperimenter{experimenter, expType, expData});

    } else {
      ByteList propData;
      io.mapRequired("data", propData);
      props.add(UnrecognizedProperty{property, propData});
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::PortModPropertyRange> {
  using iterator = ofp::detail::PortModPropertyIterator;

  static iterator begin(IO &io, ofp::detail::PortModPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    auto it = ofp::detail::PortModPropertyIterator{props.begin()};
    skip(it, end(io, range));
    return it;
  }

  static iterator end(IO &io, ofp::detail::PortModPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::PortModPropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
    skip(iter, iterEnd);
  }

  static void skip(iterator &iter, iterator iterEnd) {
    for (; iter < iterEnd; ++iter) {
      ofp::UInt16 type = iter->type();
      if (type >= ofp::OFPPMPT_UNUSED_MIN)
        break;
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::PortModPropertyList> {
  static size_t size(IO &io, ofp::detail::PortModPropertyList &props) {
    return 0;
  }

  static ofp::detail::PortModPropertyInserter &element(
      IO &io, ofp::detail::PortModPropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::PortModPropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORTMODPROPERTY_H_
