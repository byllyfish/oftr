// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YQUEUEGETCONFIGREPLY_H_
#define OFP_YAML_YQUEUEGETCONFIGREPLY_H_

#include "ofp/queuegetconfigreply.h"
#include "ofp/yaml/yqueue.h"

namespace llvm {
namespace yaml {

const char *const kQueueGetConfigReplySchema =
    R"""({Message/QueueGetConfigReply}
type: QUEUE_GET_CONFIG_REPLY
msg:
  port: PortNumber
  queues: [Queue]
)""";

template <>
struct MappingTraits<ofp::QueueGetConfigReply> {
  static void mapping(IO &io, ofp::QueueGetConfigReply &msg) {
    io.mapRequired("port", msg.port_);
    ofp::QueueRange queues = msg.queues();
    io.mapRequired("queues", queues);
  }
};

template <>
struct MappingTraits<ofp::QueueGetConfigReplyBuilder> {
  static void mapping(IO &io, ofp::QueueGetConfigReplyBuilder &msg) {
    io.mapRequired("port", msg.msg_.port_);
    io.mapRequired("queues", msg.queues_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YQUEUEGETCONFIGREPLY_H_
