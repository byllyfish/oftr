// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YTABLEMOD_H_
#define OFP_YAML_YTABLEMOD_H_

#include "ofp/tablemod.h"

namespace llvm {
namespace yaml {

const char *const kTableModSchema = R"""({Message/TableMod}
type: 'TABLE_MOD'
msg:
  table_id: UInt8
  config: UInt32
)""";

template <>
struct MappingTraits<ofp::TableMod> {
  static void mapping(IO &io, ofp::TableMod &msg) {
    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("config", msg.config_);
  }
};

template <>
struct MappingTraits<ofp::TableModBuilder> {
  static void mapping(IO &io, ofp::TableModBuilder &msg) {
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapRequired("config", msg.msg_.config_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YTABLEMOD_H_
