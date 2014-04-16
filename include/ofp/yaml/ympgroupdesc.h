#ifndef OFP_YAML_YGROUPDESC_H_
#define OFP_YAML_YGROUPDESC_H_

#include "ofp/mpgroupdesc.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPGroupDesc> {

  static void mapping(IO &io, ofp::MPGroupDesc &body) {
    io.mapRequired("type", body.type_);
    io.mapRequired("group_id", body.groupId_);

    ofp::BucketRange buckets = body.buckets();
    io.mapRequired("buckets", buckets);
  }
};

template <>
struct MappingTraits<ofp::MPGroupDescBuilder> {

  static void mapping(IO &io, ofp::MPGroupDescBuilder &body) {
    io.mapRequired("type", body.msg_.type_);
    io.mapRequired("group_id", body.msg_.groupId_);
    io.mapRequired("buckets", body.buckets_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YGROUPDESC_H_
