// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YTABLEMODPROPERTY_H_
#define OFP_YAML_YTABLEMODPROPERTY_H_

#include "ofp/tablemodproperty.h"
#include "ofp/unrecognizedproperty.h"
#include "ofp/yaml/ytypedpropertyiterator.h"

namespace ofp {
namespace detail {

struct TableModPropertyItem {};
using TableModPropertyIterator = TypedPropertyIterator<TableModPropertyItem>;
struct TableModPropertyRange {};
struct TableModPropertyInserter {};
struct TableModPropertyList {
  // Need begin/end to allow mapOptional("properties", ...)
  int begin() const { return 0; }
  int end() const { return 0; }
};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::TableModPropertyEviction> {
  static void mapping(IO &io, ofp::TableModPropertyEviction &prop) {
    io.mapRequired("flags", prop.flags_);
  }
};

template <>
struct MappingTraits<ofp::TableModPropertyVacancy> {
  static void mapping(IO &io, ofp::TableModPropertyVacancy &prop) {
    io.mapRequired("vacancy_down", prop.vacancyDown_);
    io.mapRequired("vacancy_up", prop.vacancyUp_);
    io.mapRequired("vacancy", prop.vacancy_);
  }
};

template <>
struct MappingTraits<ofp::detail::TableModPropertyItem> {
  static void mapping(IO &io, ofp::detail::TableModPropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    OFPExperimenterPropertyType property =
        static_cast<OFPExperimenterPropertyType>(elem.type());
    io.mapRequired("property", property);

    switch (property) {
      case OFP_EXPERIMENTER_PROPERTY_TYPE: {
        const TableModPropertyExperimenter &p =
            elem.property<TableModPropertyExperimenter>();

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log_debug("Unsupported TableMod Property");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::TableModPropertyInserter> {
  static void mapping(IO &io, ofp::detail::TableModPropertyInserter &list) {
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

      props.add(TableModPropertyExperimenter{experimenter, expType, expData});

    } else {
      ByteList propData;
      io.mapRequired("data", propData);
      props.add(UnrecognizedProperty{property, propData});
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::TableModPropertyRange> {
  using iterator = ofp::detail::TableModPropertyIterator;

  static iterator begin(IO &io, ofp::detail::TableModPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    auto it = ofp::detail::TableModPropertyIterator{props.begin()};
    skip(it, end(io, range));
    return it;
  }

  static iterator end(IO &io, ofp::detail::TableModPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::TableModPropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
    skip(iter, iterEnd);
  }

  static void skip(iterator &iter, iterator iterEnd) {
    for (; iter < iterEnd; ++iter) {
      ofp::UInt16 type = iter->type();
      if (type >= ofp::OFPTMPT_UNUSED_MIN || type <= ofp::OFPTMPT_UNUSED_MAX)
        break;
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::TableModPropertyList> {
  static size_t size(IO &io, ofp::detail::TableModPropertyList &props) {
    return 0;
  }

  static ofp::detail::TableModPropertyInserter &element(
      IO &io, ofp::detail::TableModPropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::TableModPropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YTABLEMODPROPERTY_H_
