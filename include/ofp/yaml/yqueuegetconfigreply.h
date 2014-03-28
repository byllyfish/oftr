#ifndef OFP_YAML_YQUEUEGETCONFIGREPLY_H_
#define OFP_YAML_YQUEUEGETCONFIGREPLY_H_

#include "ofp/queuegetconfigreply.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_QUEUE_GET_CONFIG_REPLY
// msg:
//   port: <UInt32>         { Required }
//   queues: <PacketQueue>  { Required }
//...

template <>
struct MappingTraits<ofp::QueueGetConfigReply> {

  static void mapping(IO &io, ofp::QueueGetConfigReply &msg) {
    io.mapRequired("port", msg.port_);
    // FIXME  io.mapRequired("queues", msg.queues_);
  }
};

template <>
struct MappingTraits<ofp::QueueGetConfigReplyBuilder> {

  static void mapping(IO &io, ofp::QueueGetConfigReplyBuilder &msg) {
    io.mapRequired("port", msg.msg_.port_);
    // FIXME  io.mapRequired("queues", msg.msg_.queues_);
  }
};

} // namespace yaml
} // namespace llvm

#endif // OFP_YAML_YQUEUEGETCONFIGREPLY_H_
