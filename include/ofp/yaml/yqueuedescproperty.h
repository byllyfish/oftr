// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YQUEUEDESCPROPERTY_H_
#define OFP_YAML_YQUEUEDESCPROPERTY_H_

#include "ofp/queuedescproperty.h"
#include "ofp/yaml/ytypedpropertyiterator.h"

namespace ofp {
namespace detail {

struct QueueDescPropertyItem {};
using QueueDescPropertyIterator = TypedPropertyIterator<QueueDescPropertyItem>;
struct QueueDescPropertyRange {};
struct QueueDescPropertyInserter {};
struct QueueDescPropertyList : public OptionalPropertyList {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::detail::QueueDescPropertyItem> {
  static void mapping(IO &io, ofp::detail::QueueDescPropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    OFPExperimenterPropertyType property =
        static_cast<OFPExperimenterPropertyType>(elem.type());
    io.mapRequired("property", property);

    switch (property) {
      case OFP_EXPERIMENTER_PROPERTY_TYPE: {
        const QueueDescPropertyExperimenter &p =
            elem.property<QueueDescPropertyExperimenter>();

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log_debug("Unsupported QueueDesc Property");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::QueueDescPropertyInserter> {
  static void mapping(IO &io, ofp::detail::QueueDescPropertyInserter &list) {
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

      props.add(QueueDescPropertyExperimenter{experimenter, expType, expData});

    } else {
      ByteList propData;
      io.mapRequired("data", propData);
      props.add(UnrecognizedProperty{property, propData});
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::QueueDescPropertyRange> {
  using iterator = ofp::detail::QueueDescPropertyIterator;

  static iterator begin(IO &io, ofp::detail::QueueDescPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    auto it = ofp::detail::QueueDescPropertyIterator{props.begin()};
    skip(it, end(io, range));
    return it;
  }

  static iterator end(IO &io, ofp::detail::QueueDescPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::QueueDescPropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
    skip(iter, iterEnd);
  }

  static void skip(iterator &iter, iterator iterEnd) {
    for (; iter < iterEnd; ++iter) {
      ofp::UInt16 type = iter->type();
      if (type >= ofp::OFPQDPT_UNUSED_MIN)
        break;
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::QueueDescPropertyList> {
  static size_t size(IO &io, ofp::detail::QueueDescPropertyList &props) {
    return 0;
  }

  static ofp::detail::QueueDescPropertyInserter &element(
      IO &io, ofp::detail::QueueDescPropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::QueueDescPropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YQUEUEDESCPROPERTY_H_
