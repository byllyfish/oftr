// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YQUEUEGETCONFIGREQUEST_H_
#define OFP_YAML_YQUEUEGETCONFIGREQUEST_H_

#include "ofp/queuegetconfigrequest.h"

namespace llvm {
namespace yaml {

const char *const kQueueGetConfigRequestSchema =
    R"""({Message/QueueGetConfigRequest}
type: QUEUE_GET_CONFIG_REQUEST
msg:
  port: PortNumber
)""";

template <>
struct MappingTraits<ofp::QueueGetConfigRequest> {
  static void mapping(IO &io, ofp::QueueGetConfigRequest &msg) {
    io.mapRequired("port", msg.port_);
  }
};

template <>
struct MappingTraits<ofp::QueueGetConfigRequestBuilder> {
  static void mapping(IO &io, ofp::QueueGetConfigRequestBuilder &msg) {
    io.mapRequired("port", msg.msg_.port_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YQUEUEGETCONFIGREQUEST_H_
