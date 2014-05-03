#ifndef OFP_YAML_YTABLEFEATURESPROPERTY_H_
#define OFP_YAML_YTABLEFEATURESPROPERTY_H_

#include "ofp/tablefeatureproperty.h"
#include "ofp/yaml/yinstructionids.h"
#include "ofp/yaml/yactionids.h"
#include "ofp/yaml/yoxmids.h"

namespace ofp {
namespace detail {

struct TableFeaturePropertyItem {};
struct TableFeaturePropertyRange {};
struct TableFeaturePropertyInserter {};

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
      case OFPTFPT_INSTRUCTIONS:
      case OFPTFPT_INSTRUCTIONS_MISS: {
        const TableFeaturePropertyInstructions &p = elem.property<TableFeaturePropertyInstructions>();
        InstructionIDRange instructions = p.value();
        io.mapRequired("instructions", instructions);
        break;
      }
      case OFPTFPT_EXPERIMENTER:
      case OFPTFPT_EXPERIMENTER_MISS: {
        const TableFeaturePropertyExperimenter &p = elem.property<TableFeaturePropertyExperimenter>();
        UInt32 experimenter = p.experimenter();
        //ByteRange data = p.value();

        io.mapRequired("experimenter", experimenter);
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
struct SequenceTraits<ofp::detail::TableFeaturePropertyRange> {

  static size_t size(IO &io, ofp::detail::TableFeaturePropertyRange &props) {
    ofp::PropertyRange &p = Ref_cast<ofp::PropertyRange>(props);
    return p.itemCount();
    //return p.itemCountIf([](const ofp::PropertyRange::Element &item){ return item.type() == ofp::QueuePropertyExperimenter::type(); });
  }

  static ofp::detail::TableFeaturePropertyItem &element(IO &io, ofp::detail::TableFeaturePropertyRange &props,
                                          size_t index) {
    ofp::PropertyRange &p = Ref_cast<ofp::PropertyRange>(props);

    //const ofp::PropertyRange::Element &elem = p.nthItem(index);
    return Ref_cast<ofp::detail::TableFeaturePropertyItem>(RemoveConst_cast(*p.nthItem(index)));

    //ofp::PropertyIterator iter = p.nthItemIf(index, [](const ofp::PropertyRange::Element &item){ return item.type() == ofp::QueuePropertyExperimenter::type(); });
    //const ofp::PropertyIterator::Element &elem = *iter;
    //return reinterpret_cast<ofp::detail::QueuePropertyItem &>(RemoveConst_cast(elem));
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YTABLEFEATURESPROPERTY_H_
