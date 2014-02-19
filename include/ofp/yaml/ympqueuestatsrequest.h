#ifndef OFP_YAML_YMPQUEUESTATSREQUEST_H_
#define OFP_YAML_YMPQUEUESTATSREQUEST_H_

#include "ofp/mpqueuestatsrequest.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPQueueStatsRequest> {

  static void mapping(IO &io, ofp::MPQueueStatsRequest &body) {
    io.mapRequired("port_no", body.portNo_);
    io.mapRequired("queue_id", body.queueId_);
  }
};

template <>
struct MappingTraits<ofp::MPQueueStatsRequestBuilder> {

  static void mapping(IO &io, ofp::MPQueueStatsRequestBuilder &msg) {
    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("queue_id", msg.msg_.queueId_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPQUEUESTATSREQUEST_H_
