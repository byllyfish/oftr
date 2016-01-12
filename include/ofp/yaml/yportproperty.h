// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YPORTPROPERTY_H_
#define OFP_YAML_YPORTPROPERTY_H_

#include "ofp/portproperty.h"
#include "ofp/yaml/ytypedpropertyiterator.h"
#include "ofp/unrecognizedproperty.h"

namespace ofp {
namespace detail {

struct PortPropertyItem {};
using PortPropertyIterator = TypedPropertyIterator<PortPropertyItem>;
struct PortPropertyRange {};
struct PortPropertyInserter {};
struct PortPropertyList {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

const char *const kExperimenterPropertySchema =
    R"""({Property/ExperimenterProperty}
property: EXPERIMENTER
experimenter: UInt32
exp_type: UInt32
data: HexData
)""";

template <>
struct MappingTraits<ofp::PortPropertyEthernet> {
  static void mapping(IO &io, ofp::PortPropertyEthernet &prop) {
    using namespace ofp;

    OFPPortFeaturesFlags curr = prop.curr();
    OFPPortFeaturesFlags advertised = prop.advertised();
    OFPPortFeaturesFlags supported = prop.supported();
    OFPPortFeaturesFlags peer = prop.peer();
    io.mapRequired("curr", curr);
    io.mapRequired("advertised", advertised);
    io.mapRequired("supported", supported);
    io.mapRequired("peer", peer);
    prop.setCurr(curr);
    prop.setAdvertised(advertised);
    prop.setSupported(supported);
    prop.setPeer(peer);

    io.mapRequired("curr_speed", prop.currSpeed_);
    io.mapRequired("max_speed", prop.maxSpeed_);
  }
};

template <>
struct MappingTraits<ofp::PortPropertyOptical> {
  static void mapping(IO &io, ofp::PortPropertyOptical &prop) {
    using namespace ofp;

    OFPOpticalPortFeaturesFlags supported = prop.supported();
    io.mapRequired("supported", supported);
    prop.setSupported(supported);

    io.mapRequired("tx_min_freq_lmda", prop.txMinFreqLmda_);
    io.mapRequired("tx_max_freq_lmda", prop.txMaxFreqLmda_);
    io.mapRequired("tx_grid_freq_lmda", prop.txGridFreqLmda_);
    io.mapRequired("rx_min_freq_lmda", prop.rxMinFreqLmda_);
    io.mapRequired("rx_max_freq_lmda", prop.rxMaxFreqLmda_);
    io.mapRequired("rx_grid_freq_lmda", prop.rxGridFreqLmda_);
    io.mapRequired("tx_pwr_min", prop.txPwrMin_);
    io.mapRequired("tx_pwr_max", prop.txPwrMax_);
  }
};

template <>
struct MappingTraits<ofp::detail::PortPropertyItem> {
  static void mapping(IO &io, ofp::detail::PortPropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    OFPExperimenterPropertyType property =
        static_cast<OFPExperimenterPropertyType>(elem.type());
    io.mapRequired("property", property);

    switch (property) {
      case OFP_EXPERIMENTER_PROPERTY_TYPE: {
        const PortPropertyExperimenter &p =
            elem.property<PortPropertyExperimenter>();

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log::debug("Unsupported PortPropertyItem");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::PortPropertyInserter> {
  static void mapping(IO &io, ofp::detail::PortPropertyInserter &list) {
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

      props.add(PortPropertyExperimenter{experimenter, expType, expData});

    } else {
      ByteList propData;
      io.mapRequired("data", propData);
      props.add(UnrecognizedProperty{property, propData});
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::PortPropertyRange> {
  using iterator = ofp::detail::PortPropertyIterator;

  static iterator begin(IO &io, ofp::detail::PortPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    auto it = ofp::detail::PortPropertyIterator{props.begin()};
    skip(it, end(io, range));
    return it;
  }

  static iterator end(IO &io, ofp::detail::PortPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::PortPropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
    skip(iter, iterEnd);
  }

  static void skip(iterator &iter, iterator iterEnd) {
    for (; iter < iterEnd; ++iter) {
      ofp::UInt16 type = iter->type();
      if (type >= ofp::OFPPDPT_UNUSED_MIN)
        break;
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::PortPropertyList> {
  static size_t size(IO &io, ofp::detail::PortPropertyList &props) { return 0; }

  static ofp::detail::PortPropertyInserter &element(
      IO &io, ofp::detail::PortPropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::PortPropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORTPROPERTY_H_
