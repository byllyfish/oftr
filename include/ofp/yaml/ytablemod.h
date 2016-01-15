// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YTABLEMOD_H_
#define OFP_YAML_YTABLEMOD_H_

#include "ofp/tablemod.h"
#include "ofp/yaml/ytablemodproperty.h"

namespace llvm {
namespace yaml {

const char *const kTableModSchema = R"""({Message/TableMod}
type: TABLE_MOD
msg:
  table_id: TableNumber
  config: [TableConfigFlags]
  eviction: !optout
    flags: UInt32
  vacancy: !optout
    vacancy_down: UInt8
    vacancy_up: UInt8
    vacancy: UInt8
  properties: !opt [ExperimenterProperty]
)""";

template <>
struct MappingTraits<ofp::TableMod> {
  static void mapping(IO &io, ofp::TableMod &msg) {
    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("config", msg.config_);

    ofp::PropertyRange props = msg.properties();

    auto eprop = props.findProperty(ofp::TableModPropertyEviction::type());
    if (eprop != props.end()) {
      const ofp::TableModPropertyEviction &ev =
          eprop->property<ofp::TableModPropertyEviction>();
      io.mapRequired("eviction", RemoveConst_cast(ev));
    }

    auto vprop = props.findProperty(ofp::TableModPropertyVacancy::type());
    if (vprop != props.end()) {
      const ofp::TableModPropertyVacancy &vac =
          eprop->property<ofp::TableModPropertyVacancy>();
      io.mapRequired("vacancy", RemoveConst_cast(vac));
    }

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::TableModPropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::TableModBuilder> {
  static void mapping(IO &io, ofp::TableModBuilder &msg) {
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapRequired("config", msg.msg_.config_);

    Optional<ofp::TableModPropertyEviction> optEviction;
    io.mapOptional("eviction", optEviction);

    Optional<ofp::TableModPropertyVacancy> optVacancy;
    io.mapOptional("vacancy", optVacancy);

    ofp::PropertyList props;

    if (optEviction)
      props.add(*optEviction);

    if (optVacancy)
      props.add(*optVacancy);

    io.mapOptional("properties",
                   Ref_cast<ofp::detail::TableModPropertyList>(props));
    msg.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YTABLEMOD_H_
