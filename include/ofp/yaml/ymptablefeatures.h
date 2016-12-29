// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPTABLEFEATURES_H_
#define OFP_YAML_YMPTABLEFEATURES_H_

#include "ofp/mptablefeatures.h"
#include "ofp/yaml/ytablefeatureproperty.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPTableFeatures> {
  static void mapping(IO &io, ofp::MPTableFeatures &body) {
    using namespace ofp;

    io.mapRequired("table_id", body.tableId_);
    io.mapRequired("name", body.name_);
    io.mapRequired("metadata_match", body.metadataMatch_);
    io.mapRequired("metadata_write", body.metadataWrite_);
    io.mapRequired("config", body.config_);
    io.mapRequired("max_entries", body.maxEntries_);

    PropertyRange props = body.properties();

    InstructionIDRange ins =
        props.valueRange<TableFeaturePropertyInstructions>();
    io.mapRequired("instructions", ins);

    InstructionIDRange insMiss =
        props.valueRange<TableFeaturePropertyInstructionsMiss>(ins);
    io.mapOptional("instructions_miss", insMiss, ins);

    TableIDRange nextTables =
        props.valueRange<TableFeaturePropertyNextTables>();
    io.mapRequired("next_tables", nextTables);

    TableIDRange nextTablesMiss =
        props.valueRange<TableFeaturePropertyNextTablesMiss>(nextTables);
    io.mapOptional("next_tables_miss", nextTablesMiss, nextTables);

    ActionIDRange writeAct =
        props.valueRange<TableFeaturePropertyWriteActions>();
    io.mapRequired("write_actions", writeAct);

    ActionIDRange writeActMiss =
        props.valueRange<TableFeaturePropertyWriteActionsMiss>(writeAct);
    io.mapOptional("write_actions_miss", writeActMiss, writeAct);

    ActionIDRange applyAct =
        props.valueRange<TableFeaturePropertyApplyActions>();
    io.mapRequired("apply_actions", applyAct);

    ActionIDRange applyActMiss =
        props.valueRange<TableFeaturePropertyApplyActionsMiss>(applyAct);
    io.mapOptional("apply_actions_miss", applyActMiss, applyAct);

    OXMIDRange match = props.valueRange<TableFeaturePropertyMatch>();
    io.mapRequired("match", match);

    OXMIDRange wildcards = props.valueRange<TableFeaturePropertyWildcards>();
    io.mapRequired("wildcards", wildcards);

    OXMIDRange writeSetField =
        props.valueRange<TableFeaturePropertyWriteSetField>();
    io.mapRequired("write_set_field", writeSetField);

    OXMIDRange writeSetFieldMiss =
        props.valueRange<TableFeaturePropertyWriteSetFieldMiss>(writeSetField);
    io.mapOptional("write_set_field_miss", writeSetFieldMiss, writeSetField);

    OXMIDRange applySetField =
        props.valueRange<TableFeaturePropertyApplySetField>();
    io.mapRequired("apply_set_field", applySetField);

    OXMIDRange applySetFieldMiss =
        props.valueRange<TableFeaturePropertyApplySetFieldMiss>(applySetField);
    io.mapOptional("apply_set_field_miss", applySetFieldMiss, applySetField);

    io.mapRequired("properties",
                   Ref_cast<ofp::detail::TableFeaturePropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::MPTableFeaturesBuilder> {
  static void mapping(IO &io, ofp::MPTableFeaturesBuilder &body) {
    using namespace ofp;
    using namespace ofp::detail;

    io.mapRequired("table_id", body.msg_.tableId_);
    io.mapRequired("name", body.msg_.name_);
    io.mapRequired("metadata_match", body.msg_.metadataMatch_);
    io.mapRequired("metadata_write", body.msg_.metadataWrite_);
    io.mapRequired("config", body.msg_.config_);
    io.mapRequired("max_entries", body.msg_.maxEntries_);

    PropertyList props;

    InstructionIDList ins;
    io.mapRequired("instructions", ins);
    props.add(TableFeaturePropertyInstructions{ins.toRange()});

    Optional<InstructionIDList> insMiss;
    io.mapOptional("instructions_miss", insMiss);
    if (insMiss) {
      props.add(TableFeaturePropertyInstructionsMiss{insMiss->toRange()});
    }

    TableIDList tables;
    io.mapRequired("next_tables", tables);
    props.add(TableFeaturePropertyNextTables{tables.toRange()});

    Optional<TableIDList> tablesMiss;
    io.mapOptional("next_tables_miss", tablesMiss);
    if (tablesMiss) {
      props.add(TableFeaturePropertyNextTablesMiss{tablesMiss->toRange()});
    }

    ActionIDList act;
    io.mapRequired("write_actions", act);
    props.add(TableFeaturePropertyWriteActions{act.toRange()});

    Optional<ActionIDList> actMiss;
    io.mapOptional("write_actions_miss", actMiss);
    if (actMiss) {
      props.add(TableFeaturePropertyWriteActionsMiss{actMiss->toRange()});
    }

    act.clear();
    io.mapRequired("apply_actions", act);
    props.add(TableFeaturePropertyApplyActions{act.toRange()});

    actMiss.reset();
    io.mapOptional("apply_actions_miss", actMiss);
    if (actMiss) {
      props.add(TableFeaturePropertyApplyActionsMiss{actMiss->toRange()});
    }

    OXMIDList match;
    io.mapRequired("match", match);
    props.add(TableFeaturePropertyMatch{match.toRange()});

    match.clear();
    io.mapRequired("wildcards", match);
    props.add(TableFeaturePropertyWildcards{match.toRange()});

    match.clear();
    io.mapRequired("write_set_field", match);
    props.add(TableFeaturePropertyWriteSetField{match.toRange()});

    Optional<OXMIDList> matchMiss;
    io.mapOptional("write_set_field_miss", matchMiss);
    if (matchMiss) {
      props.add(TableFeaturePropertyWriteSetFieldMiss{matchMiss->toRange()});
    }

    match.clear();
    io.mapRequired("apply_set_field", match);
    props.add(TableFeaturePropertyApplySetField{match.toRange()});

    matchMiss.reset();
    io.mapOptional("apply_set_field_miss", matchMiss);
    if (matchMiss) {
      props.add(TableFeaturePropertyApplySetFieldMiss{matchMiss->toRange()});
    }

    io.mapOptional("properties", Ref_cast<TableFeaturePropertyList>(props));

    body.setProperties(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPTABLEFEATURES_H_
