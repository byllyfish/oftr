// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YQUEUEPROPERTY_H_
#define OFP_YAML_YQUEUEPROPERTY_H_

#include "ofp/yaml/ytypedpropertyiterator.h"

namespace ofp {
namespace detail {

struct QueuePropertyItem {};
using QueuePropertyIterator = TypedPropertyIterator<QueuePropertyItem>;
struct QueuePropertyRange {};
struct QueuePropertyList : public OptionalPropertyList {};
struct QueuePropertyInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::detail::QueuePropertyItem> {
  static void mapping(IO &io, ofp::detail::QueuePropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);

    if (elem.type() == QueuePropertyExperimenter::type()) {
      auto &p = elem.property<QueuePropertyExperimenter>();

      Hex32 experimenter = p.experimenter();
      ByteRange data = p.value();

      io.mapRequired("experimenter", experimenter);
      io.mapRequired("value", data);

    } else {
      log_debug("Unsupported QueuePropertyItem");
      UInt32 value = 0;
      io.mapRequired("UNKNOWN", value);
    }
  }
};

template <>
struct MappingTraits<ofp::detail::QueuePropertyInserter> {
  static void mapping(IO &io, ofp::detail::QueuePropertyInserter &inserter) {
    using namespace ofp;

    PropertyList &props = Ref_cast<PropertyList>(inserter);

    UInt32 experimenter;
    io.mapRequired("experimenter", experimenter);

    ByteList data;
    io.mapRequired("value", data);

    props.add(QueuePropertyExperimenter{experimenter, data});
  }
};

template <>
struct SequenceTraits<ofp::detail::QueuePropertyRange> {
  using iterator = ofp::detail::QueuePropertyIterator;

  static iterator begin(IO &io, ofp::detail::QueuePropertyRange &range) {
    auto props = Ref_cast<ofp::PropertyRange>(range);
    auto it = ofp::detail::QueuePropertyIterator{props.begin()};
    skip(it, end(io, range));
    return it;
  }

  static iterator end(IO &io, ofp::detail::QueuePropertyRange &range) {
    auto props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::QueuePropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
    skip(iter, iterEnd);
  }

  static void skip(iterator &iter, iterator iterEnd) {
    for (; iter < iterEnd; ++iter) {
      if (iter->type() >= ofp::OFPQT_UNUSED_MIN)
        break;
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::QueuePropertyList> {
  static size_t size(IO &io, ofp::detail::QueuePropertyList &list) { return 0; }

  static ofp::detail::QueuePropertyInserter &element(
      IO &io, ofp::detail::QueuePropertyList &list, size_t index) {
    return Ref_cast<ofp::detail::QueuePropertyInserter>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YQUEUEPROPERTY_H_
