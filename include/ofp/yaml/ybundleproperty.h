// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YBUNDLEPROPERTY_H_
#define OFP_YAML_YBUNDLEPROPERTY_H_

#include "ofp/bundleproperty.h"
#include "ofp/unrecognizedproperty.h"
#include "ofp/yaml/ytypedpropertyiterator.h"

namespace ofp {
namespace detail {

struct BundlePropertyItem {};
using BundlePropertyIterator = TypedPropertyIterator<BundlePropertyItem>;
struct BundlePropertyRange {};
struct BundlePropertyInserter {};
struct BundlePropertyList {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::detail::BundlePropertyItem> {
  static void mapping(IO &io, ofp::detail::BundlePropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    OFPExperimenterPropertyType property =
        static_cast<OFPExperimenterPropertyType>(elem.type());
    io.mapRequired("property", property);

    switch (property) {
      case OFP_EXPERIMENTER_PROPERTY_TYPE: {
        const BundlePropertyExperimenter &p =
            elem.property<BundlePropertyExperimenter>();

        assert(p.size() >= BundlePropertyExperimenter::FixedHeaderSize);

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log::debug("Unsupported Bundle Property");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::BundlePropertyInserter> {
  static void mapping(IO &io, ofp::detail::BundlePropertyInserter &list) {
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

      props.add(BundlePropertyExperimenter{experimenter, expType, expData});

    } else {
      ByteList propData;
      io.mapRequired("data", propData);
      props.add(UnrecognizedProperty{property, propData});
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::BundlePropertyRange> {
  using iterator = ofp::detail::BundlePropertyIterator;

  static iterator begin(IO &io, ofp::detail::BundlePropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::BundlePropertyIterator{props.begin()};
  }

  static iterator end(IO &io, ofp::detail::BundlePropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::BundlePropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) { ++iter; }
};

template <>
struct SequenceTraits<ofp::detail::BundlePropertyList> {
  static size_t size(IO &io, ofp::detail::BundlePropertyList &props) {
    return 0;
  }

  static ofp::detail::BundlePropertyInserter &element(
      IO &io, ofp::detail::BundlePropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::BundlePropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBUNDLEPROPERTY_H_
