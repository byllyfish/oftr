// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YASYNCCONFIGPROPERTY_H_
#define OFP_YAML_YASYNCCONFIGPROPERTY_H_

#include "ofp/asyncconfigproperty.h"
#include "ofp/yaml/ytypedpropertyiterator.h"
#include "ofp/unrecognizedproperty.h"

namespace ofp {
namespace detail {

struct AsyncConfigPropertyItem {};
using AsyncConfigPropertyIterator =
    TypedPropertyIterator<AsyncConfigPropertyItem>;
struct AsyncConfigPropertyRange {};
struct AsyncConfigPropertyInserter {};
struct AsyncConfigPropertyList {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

const char *const kAsyncConfigExperimenterPropertySchema =
    R"""({Property/AsyncConfigExperimenterProperty}
property: 'EXPERIMENTER_SLAVE' | 'EXPERIMENTER_MASTER'
experimenter: UInt32
exp_type: UInt32
data: HexString
)""";

template <>
struct MappingTraits<ofp::detail::AsyncConfigPropertyItem> {
  static void mapping(IO &io, ofp::detail::AsyncConfigPropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    Hex16 property = elem.type();
    io.mapRequired("property", property);

    switch (property) {
      case OFPACPT_EXPERIMENTER_SLAVE:
      case OFPACPT_EXPERIMENTER_MASTER: {
        const AsyncConfigPropertyExperimenterMaster &p =
            elem.property<AsyncConfigPropertyExperimenterMaster>();

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log::debug("Unsupported AsyncConfigPropertyItem");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::AsyncConfigPropertyInserter> {
  static void mapping(IO &io, ofp::detail::AsyncConfigPropertyInserter &list) {
    using namespace ofp;

    PropertyList &props = Ref_cast<PropertyList>(list);

    UInt16 property;
    io.mapRequired("property", property);

    if (property >= OFPACPT_EXPERIMENTER_SLAVE) {
      UInt32 experimenter;
      io.mapRequired("experimenter", experimenter);

      UInt32 expType;
      io.mapRequired("exp_type", expType);

      ByteList expData;
      io.mapRequired("data", expData);

      switch (property) {
        case OFPACPT_EXPERIMENTER_SLAVE:
          props.add(AsyncConfigPropertyExperimenterSlave{experimenter, expType,
                                                         expData});
          break;
        case OFPACPT_EXPERIMENTER_MASTER:
          props.add(AsyncConfigPropertyExperimenterMaster{experimenter, expType,
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
struct SequenceTraits<ofp::detail::AsyncConfigPropertyRange> {
  using iterator = ofp::detail::AsyncConfigPropertyIterator;

  static iterator begin(IO &io, ofp::detail::AsyncConfigPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    auto it = ofp::detail::AsyncConfigPropertyIterator{props.begin()};
    skip(it, end(io, range));
    return it;
  }

  static iterator end(IO &io, ofp::detail::AsyncConfigPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::AsyncConfigPropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
    skip(iter, iterEnd);
  }

  static void skip(iterator &iter, iterator iterEnd) {
    for (; iter < iterEnd; ++iter) {
      ofp::UInt16 type = iter->type();
      if (type >= ofp::OFPACPT_UNUSED_MIN)
        break;
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::AsyncConfigPropertyList> {
  static size_t size(IO &io, ofp::detail::AsyncConfigPropertyList &props) {
    return 0;
  }

  static ofp::detail::AsyncConfigPropertyInserter &element(
      IO &io, ofp::detail::AsyncConfigPropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::AsyncConfigPropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YASYNCCONFIGPROPERTY_H_
