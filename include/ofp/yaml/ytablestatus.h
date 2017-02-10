// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YTABLESTATUS_H_
#define OFP_YAML_YTABLESTATUS_H_

#include "ofp/tablestatus.h"
#include "ofp/yaml/yconstants.h"
#include "ofp/yaml/ytabledesc.h"

namespace llvm {
namespace yaml {

const char *const kTableStatusSchema = R"""({Message/TableStatus}
type: TABLE_STATUS
msg:
  reason: TableStatusReason
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
struct MappingTraits<ofp::TableStatus> {
  static void mapping(IO &io, ofp::TableStatus &msg) {
    io.mapRequired("reason", msg.reason_);

    ofp::TableDesc &table = RemoveConst_cast(msg.table());
    MappingTraits<ofp::TableDesc>::mapping(io, table);
  }
};

template <>
struct MappingTraits<ofp::TableStatusBuilder> {
  static void mapping(IO &io, ofp::TableStatusBuilder &msg) {
    io.mapRequired("reason", msg.msg_.reason_);
    MappingTraits<ofp::TableDescBuilder>::mapping(io, msg.table_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YTABLESTATUS_H_
