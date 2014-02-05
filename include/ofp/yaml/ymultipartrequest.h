//  ===== ---- ofp/yaml/ymultipartrequest.h ----------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the MappingTraits for the MultipartRequest and
/// MultipartRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_MULTIPARTREQUEST_H_
#define OFP_YAML_MULTIPARTREQUEST_H_

#include "ofp/multipartrequest.h"
#include "ofp/mpflowstatsrequest.h"
#include "ofp/yaml/yflowstatsrequest.h"
#include "ofp/memorychannel.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MultipartRequest> {

  static void mapping(IO &io, ofp::MultipartRequest &msg) {
    using namespace ofp;

    OFPMultipartType type = msg.requestType();
    io.mapRequired("type", type);
    io.mapRequired("flags", msg.flags_);

    switch (type) {
    case OFPMP_DESC:
    case OFPMP_TABLE:
    case OFPMP_PORT_DESC:
      // empty request body
      break;
    case OFPMP_FLOW:
    case OFPMP_AGGREGATE: {
      const MPFlowStatsRequest *stats = MPFlowStatsRequest::cast(&msg);
      if (stats) {
        io.mapRequired("body", RemoveConst_cast(*stats));
      }
      break;
    }
    default:
      // FIXME - implement the rest.
      log::debug("MultipartRequest MappingTraits not fully implemented.");
      break;
    }
  }
};

template <>
struct MappingTraits<ofp::MultipartRequestBuilder> {

  static void mapping(IO &io, ofp::MultipartRequestBuilder &msg) {
    using namespace ofp;

    OFPMultipartType type;
    io.mapRequired("type", type);
    io.mapRequired("flags", msg.msg_.flags_);
    msg.setRequestType(type);

    switch (type) {
    case OFPMP_DESC:
    case OFPMP_TABLE:
    case OFPMP_PORT_DESC:
      // empty request body
      break;
    case OFPMP_FLOW:
    case OFPMP_AGGREGATE: {
      MPFlowStatsRequestBuilder stats;
      io.mapRequired("body", stats);
      MemoryChannel channel{msg.msg_.header_.version()};
      stats.write(&channel);
      msg.setRequestBody(channel.data(), channel.size());
      break;
    }
    default:
      // FIXME - implement the rest.
      log::debug(
          "MultipartRequestBuilder MappingTraits not fully implemented.");
      break;
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_MULTIPARTREQUEST_H_
