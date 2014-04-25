#ifndef OFP_YAML_YMPTABLEFEATURES_H_
#define OFP_YAML_YMPTABLEFEATURES_H_

#include "ofp/mptablefeatures.h"

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

    //ofp::PropertyRange properties = body.properties();
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
