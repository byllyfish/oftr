#ifndef OFP_YAML_YTABLEFEATURESPROPERTY_H_
#define OFP_YAML_YTABLEFEATURESPROPERTY_H_

#include "ofp/tablefeatureproperty.h"
#include "ofp/yaml/yinstructionids.h"
#include "ofp/yaml/yactionids.h"
#include "ofp/yaml/yoxmids.h"
#include "ofp/yaml/ytableids.h"

namespace ofp {
namespace detail {

struct TableFeaturePropertyItem {};
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
    UInt16 property = elem.type();
    io.mapRequired("property", property);

    switch (property) {
      case OFPTFPT_EXPERIMENTER:
      case OFPTFPT_EXPERIMENTER_MISS: {
        const TableFeaturePropertyExperimenter &p = elem.property<TableFeaturePropertyExperimenter>();

        UInt32 experimenter = p.experimenter();
        io.mapRequired("experimenter_id", experimenter);

        UInt32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("exp_data", expData);
        //io.mapRequired("value", data);
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

    UInt32 experimenter;
    io.mapRequired("experimenter_id", experimenter);

    UInt32 expType;
    io.mapRequired("exp_type", expType);

    ByteList expData;
    io.mapRequired("exp_data", expData);

    switch (property) {
      case OFPTFPT_EXPERIMENTER: {
        props.add(TableFeaturePropertyExperimenter{experimenter, expType, expData});
        break;
      }
      case OFPTFPT_EXPERIMENTER_MISS: {
        props.add(TableFeaturePropertyExperimenterMiss{experimenter, expType, expData});
        break;
      }
      default: {
        log::debug("Unsupported TableFeaturePropertyInserter");
        //ByteRange data = elem.value();
        //io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::TableFeaturePropertyRange> {

  static size_t size(IO &io, ofp::detail::TableFeaturePropertyRange &props) {
    ofp::PropertyRange &p = Ref_cast<ofp::PropertyRange>(props);
    return p.itemCountIf([](const ofp::PropertyRange::Element &item){ return (item.type() == ofp::OFPTFPT_EXPERIMENTER) || (item.type() == ofp::OFPTFPT_EXPERIMENTER_MISS); });
  }

  static ofp::detail::TableFeaturePropertyItem &element(IO &io, ofp::detail::TableFeaturePropertyRange &props,
                                          size_t index) {
    ofp::PropertyRange &p = Ref_cast<ofp::PropertyRange>(props);
    ofp::PropertyIterator iter = p.nthItemIf(index, [](const ofp::PropertyRange::Element &item){ return (item.type() == ofp::OFPTFPT_EXPERIMENTER) || (item.type() == ofp::OFPTFPT_EXPERIMENTER_MISS); });
    return Ref_cast<ofp::detail::TableFeaturePropertyItem>(RemoveConst_cast(*iter));
  }
};

template <>
struct SequenceTraits<ofp::detail::TableFeaturePropertyList> {

  static size_t size(IO &io, ofp::detail::TableFeaturePropertyList &props) {
    return 0;
  }

  static ofp::detail::TableFeaturePropertyInserter &element(IO &io, ofp::detail::TableFeaturePropertyList &props,
                                          size_t index) {
     return Ref_cast<ofp::detail::TableFeaturePropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YTABLEFEATURESPROPERTY_H_
