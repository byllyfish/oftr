#ifndef OFP_YAML_YMPTABLEFEATURES_H_
#define OFP_YAML_YMPTABLEFEATURES_H_

#include "ofp/mptablefeatures.h"
#include "ofp/yaml/ytablefeatureproperty.h"

namespace llvm {
namespace yaml {


template <>
struct MappingTraits<ofp::MPTableFeatures> {

  static void mapping(IO &io, ofp::MPTableFeatures &body) {
    io.mapRequired("table_id", body.tableId_);
    io.mapRequired("name", body.name_);
    io.mapRequired("metadata_match", body.metadataMatch_);
    io.mapRequired("metadata_write", body.metadataWrite_);
    io.mapRequired("config", body.config_);
    io.mapRequired("max_entries", body.maxEntries_);

    ofp::PropertyRange props = body.properties();

    ofp::InstructionIDRange ins = props.valueRange<ofp::TableFeaturePropertyInstructions>();
    io.mapRequired("instructions", ins);
    
    ofp::InstructionIDRange insMiss = props.valueRange<ofp::TableFeaturePropertyInstructionsMiss>(ins);
    io.mapOptional("instructions_miss", insMiss, ins);
   
    ofp::ByteRange nextTables = props.valueRange<ofp::TableFeaturePropertyNextTables>();
    io.mapRequired("next_tables", nextTables);

    ofp::ByteRange nextTablesMiss = props.valueRange<ofp::TableFeaturePropertyNextTablesMiss>(nextTables);
    io.mapOptional("next_tables_miss", nextTablesMiss, nextTables);

    ofp::ActionIDRange writeAct = props.valueRange<ofp::TableFeaturePropertyWriteActions>();
    io.mapRequired("write_actions", writeAct);

    ofp::ActionIDRange writeActMiss = props.valueRange<ofp::TableFeaturePropertyWriteActionsMiss>(writeAct);
    io.mapOptional("write_actions_miss", writeActMiss, writeAct);

    ofp::ActionIDRange applyAct = props.valueRange<ofp::TableFeaturePropertyApplyActions>();
    io.mapRequired("apply_actions", applyAct);

    ofp::ActionIDRange applyActMiss = props.valueRange<ofp::TableFeaturePropertyApplyActionsMiss>(applyAct);
    io.mapOptional("apply_actions_miss", applyActMiss, applyAct);

    ofp::OXMIDRange match = props.valueRange<ofp::TableFeaturePropertyMatch>();
    io.mapRequired("match", match);

    ofp::OXMIDRange wildcards = props.valueRange<ofp::TableFeaturePropertyWildcards>();
    io.mapRequired("wildcards", wildcards);

    ofp::OXMIDRange writeSetField = props.valueRange<ofp::TableFeaturePropertyWriteSetField>();
    io.mapRequired("write_set_field", writeSetField);

    ofp::OXMIDRange writeSetFieldMiss = props.valueRange<ofp::TableFeaturePropertyWriteSetFieldMiss>(writeSetField);
    io.mapOptional("write_set_field_miss", writeSetFieldMiss, writeSetField);

    ofp::OXMIDRange applySetField = props.valueRange<ofp::TableFeaturePropertyApplySetField>();
    io.mapRequired("apply_set_field", applySetField);

    ofp::OXMIDRange applySetFieldMiss = props.valueRange<ofp::TableFeaturePropertyApplySetFieldMiss>(applySetField);
    io.mapOptional("apply_set_field_miss", applySetFieldMiss, applySetField);

    io.mapRequired("properties", Ref_cast<ofp::detail::TableFeaturePropertyRange>(props));

    //io.mapRequired("properties", properties);
  }
};

template <>
struct MappingTraits<ofp::MPTableFeaturesBuilder> {

  static void mapping(IO &io, ofp::MPTableFeaturesBuilder &body) {
    io.mapRequired("table_id", body.msg_.tableId_);
    io.mapRequired("name", body.msg_.name_);
    io.mapRequired("metadata_match", body.msg_.metadataMatch_);
    io.mapRequired("metadata_write", body.msg_.metadataWrite_);
    io.mapRequired("config", body.msg_.config_);
    io.mapRequired("max_entries", body.msg_.maxEntries_);

    //io.mapRequired("properties", body.properties_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YMPTABLEFEATURES_H_
