// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMPQUEUEDESCREQUEST_H_
#define OFP_YAML_YMPQUEUEDESCREQUEST_H_

#include "ofp/mpqueuedescrequest.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPQueueDescRequest> {
  static void mapping(IO &io, ofp::MPQueueDescRequest &body) {
    io.mapRequired("port_no", body.portNo_);
    io.mapRequired("queue_id", body.queueId_);
  }
};

template <>
struct MappingTraits<ofp::MPQueueDescRequestBuilder> {
  static void mapping(IO &io, ofp::MPQueueDescRequestBuilder &msg) {
    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("queue_id", msg.msg_.queueId_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPQUEUEDESCREQUEST_H_
