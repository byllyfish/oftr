#ifndef OFP_YAML_TABLESTATUS_H_
#define OFP_YAML_TABLESTATUS_H_

#include "ofp/tablestatus.h"
#include "ofp/yaml/ytabledesc.h"
#include "ofp/yaml/yconstants.h"

namespace llvm {
namespace yaml {

const char *const kTableStatusSchema = R"""({Message/TableStatus}
type: TABLE_STATUS
msg:
  reason: TableStatusReason
  table: TableDesc
)""";

template <>
struct MappingTraits<ofp::TableStatus> {
  static void mapping(IO &io, ofp::TableStatus &msg) {
    io.mapRequired("reason", msg.reason_);

    ofp::TableDesc &table = RemoveConst_cast(msg.table());
    io.mapRequired("table", table);
  }
};

template <>
struct MappingTraits<ofp::TableStatusBuilder> {
  static void mapping(IO &io, ofp::TableStatusBuilder &msg) {
    io.mapRequired("reason", msg.msg_.reason_);
    io.mapRequired("table", msg.table_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_TABLESTATUS_H_
