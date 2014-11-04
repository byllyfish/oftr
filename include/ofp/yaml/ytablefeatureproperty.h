// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YTABLEFEATURESPROPERTY_H_
#define OFP_YAML_YTABLEFEATURESPROPERTY_H_

#include "ofp/tablefeatureproperty.h"
#include "ofp/unrecognizedproperty.h"
#include "ofp/yaml/yinstructionids.h"
#include "ofp/yaml/yactionids.h"
#include "ofp/yaml/yoxmids.h"
#include "ofp/yaml/ytableids.h"
#include "ofp/yaml/ytypedpropertyiterator.h"

namespace ofp {
namespace detail {

struct TableFeaturePropertyItem {};
using TableFeaturePropertyIterator =
    TypedPropertyIterator<TableFeaturePropertyItem>;
struct TableFeaturePropertyRange {};
struct TableFeaturePropertyInserter {};
struct TableFeaturePropertyList {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::detail::TableFeaturePropertyItem> {
  static void mapping(IO &io, ofp::detail::TableFeaturePropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    Hex16 property = elem.type();
    io.mapRequired("property", property);

    switch (property) {
      case OFPTFPT_EXPERIMENTER:
      case OFPTFPT_EXPERIMENTER_MISS: {
        const TableFeaturePropertyExperimenter &p =
            elem.property<TableFeaturePropertyExperimenter>();

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log::debug("Unsupported TableFeaturePropertyItem");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::TableFeaturePropertyInserter> {
  static void mapping(IO &io, ofp::detail::TableFeaturePropertyInserter &list) {
    using namespace ofp;

    PropertyList &props = Ref_cast<PropertyList>(list);

    UInt16 property;
    io.mapRequired("property", property);

    if (property >= OFPTFPT_EXPERIMENTER) {
      UInt32 experimenter;
      io.mapRequired("experimenter", experimenter);

      UInt32 expType;
      io.mapRequired("exp_type", expType);

      ByteList expData;
      io.mapRequired("data", expData);

      switch (property) {
        case OFPTFPT_EXPERIMENTER:
          props.add(
              TableFeaturePropertyExperimenter{experimenter, expType, expData});
          break;
        case OFPTFPT_EXPERIMENTER_MISS:
          props.add(TableFeaturePropertyExperimenterMiss{experimenter, expType,
                                                         expData});
          break;
      }

    } else {
      ByteList propData;
      io.mapRequired("data", propData);
      props.add(UnrecognizedProperty{property, propData});
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::TableFeaturePropertyRange> {
  using iterator = ofp::detail::TableFeaturePropertyIterator;

  static iterator begin(IO &io, ofp::detail::TableFeaturePropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    auto it = ofp::detail::TableFeaturePropertyIterator{props.begin()};
    skip(it, end(io, range));
    return it;
  }

  static iterator end(IO &io, ofp::detail::TableFeaturePropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::TableFeaturePropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
    skip(iter, iterEnd);
  }

  static void skip(iterator &iter, iterator iterEnd) {
    for (; iter < iterEnd; ++iter) {
      ofp::UInt16 type = iter->type();
      if (type >= ofp::OFPTFPT_UNUSED_MIN || type == ofp::OFPTFPT_UNUSED_9 ||
          type == ofp::OFPTFPT_UNUSED_11)
        break;
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::TableFeaturePropertyList> {
  static size_t size(IO &io, ofp::detail::TableFeaturePropertyList &props) {
    return 0;
  }

  static ofp::detail::TableFeaturePropertyInserter &element(
      IO &io, ofp::detail::TableFeaturePropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::TableFeaturePropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YTABLEFEATURESPROPERTY_H_
