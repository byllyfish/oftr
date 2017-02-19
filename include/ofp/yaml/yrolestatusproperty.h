// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YROLESTATUSPROPERTY_H_
#define OFP_YAML_YROLESTATUSPROPERTY_H_

#include "ofp/rolestatusproperty.h"
#include "ofp/unrecognizedproperty.h"
#include "ofp/yaml/ytypedpropertyiterator.h"

namespace ofp {
namespace detail {

struct RoleStatusPropertyItem {};
using RoleStatusPropertyIterator =
    TypedPropertyIterator<RoleStatusPropertyItem>;
struct RoleStatusPropertyRange {};
struct RoleStatusPropertyInserter {};
struct RoleStatusPropertyList : public OptionalPropertyList {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::detail::RoleStatusPropertyItem> {
  static void mapping(IO &io, ofp::detail::RoleStatusPropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    OFPExperimenterPropertyType property =
        static_cast<OFPExperimenterPropertyType>(elem.type());
    io.mapRequired("property", property);

    switch (property) {
      case OFP_EXPERIMENTER_PROPERTY_TYPE: {
        const RoleStatusPropertyExperimenter &p =
            elem.property<RoleStatusPropertyExperimenter>();

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log_debug("Unsupported RoleStatus Property");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::RoleStatusPropertyInserter> {
  static void mapping(IO &io, ofp::detail::RoleStatusPropertyInserter &list) {
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

      props.add(RoleStatusPropertyExperimenter{experimenter, expType, expData});

    } else {
      ByteList propData;
      io.mapRequired("data", propData);
      props.add(UnrecognizedProperty{property, propData});
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::RoleStatusPropertyRange> {
  using iterator = ofp::detail::RoleStatusPropertyIterator;

  static iterator begin(IO &io, ofp::detail::RoleStatusPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::RoleStatusPropertyIterator{props.begin()};
  }

  static iterator end(IO &io, ofp::detail::RoleStatusPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::RoleStatusPropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) { ++iter; }
};

template <>
struct SequenceTraits<ofp::detail::RoleStatusPropertyList> {
  static size_t size(IO &io, ofp::detail::RoleStatusPropertyList &props) {
    return 0;
  }

  static ofp::detail::RoleStatusPropertyInserter &element(
      IO &io, ofp::detail::RoleStatusPropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::RoleStatusPropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YROLESTATUSPROPERTY_H_
