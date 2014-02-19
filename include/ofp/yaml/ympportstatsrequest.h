#ifndef OFP_YAML_YMPPORTSTATSREQUEST_H_
#define OFP_YAML_YMPPORTSTATSREQUEST_H_

#include "ofp/mpportstatsrequest.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPPortStatsRequest> {

  static void mapping(IO &io, ofp::MPPortStatsRequest &body) {
    io.mapRequired("port_no", body.portNo_);
  }
};

template <>
struct MappingTraits<ofp::MPPortStatsRequestBuilder> {

  static void mapping(IO &io, ofp::MPPortStatsRequestBuilder &msg) {
    io.mapRequired("port_no", msg.msg_.portNo_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPPORTSTATSREQUEST_H_
