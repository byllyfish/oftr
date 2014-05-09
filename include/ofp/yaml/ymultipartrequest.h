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

#ifndef OFP_YAML_YMULTIPARTREQUEST_H_
#define OFP_YAML_YMULTIPARTREQUEST_H_

#include "ofp/multipartrequest.h"
#include "ofp/mpflowstatsrequest.h"
#include "ofp/yaml/ympflowstatsrequest.h"
#include "ofp/yaml/ympportstatsrequest.h"
#include "ofp/yaml/ympqueuestatsrequest.h"
#include "ofp/yaml/ympmeterconfigrequest.h"
#include "ofp/yaml/ymptablefeatures.h"
#include "ofp/yaml/ympgroupstatsrequest.h"
#include "ofp/yaml/ympexperimenter.h"
#include "ofp/yaml/ympreplyseq.h"
#include "ofp/mpmeterstatsrequest.h"
#include "ofp/memorychannel.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_MULTIPART_REQUEST
// msg:
//   type: <OFPMultipartType>    { Required }
//   flags: <UInt16>             { Required }
//   body:
//     { Depends on OFPMultipartType }
//...

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
      case OFPMP_GROUP_DESC:
      case OFPMP_GROUP_FEATURES:
      case OFPMP_METER_FEATURES:
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
      case OFPMP_PORT_STATS: {
        const MPPortStatsRequest *stats = MPPortStatsRequest::cast(&msg);
        if (stats) {
          io.mapRequired("body", RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_QUEUE: {
        const MPQueueStatsRequest *stats = MPQueueStatsRequest::cast(&msg);
        if (stats) {
          io.mapRequired("body", RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_METER_CONFIG: {
        const MPMeterConfigRequest *stats = MPMeterConfigRequest::cast(&msg);
        if (stats) {
          io.mapRequired("body", RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_METER: {
        const MPMeterStatsRequest *stats = MPMeterStatsRequest::cast(&msg);
        if (stats) {
          io.mapRequired("body", RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_GROUP: {
        const MPGroupStatsRequest *stats = MPGroupStatsRequest::cast(&msg);
        if (stats) {
          io.mapRequired("body", RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_TABLE_FEATURES: {
        ofp::detail::MPReplyVariableSizeSeq<MPTableFeatures> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_EXPERIMENTER: {
        const MPExperimenter *stats = MPExperimenter::cast(&msg);
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

    OFPMultipartType type = OFPMP_DESC;
    io.mapRequired("type", type);
    io.mapRequired("flags", msg.msg_.flags_);
    msg.setRequestType(type);

    switch (type) {
      case OFPMP_DESC:
      case OFPMP_TABLE:
      case OFPMP_PORT_DESC:
      case OFPMP_GROUP_DESC:
      case OFPMP_GROUP_FEATURES:
      case OFPMP_METER_FEATURES:
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
      case OFPMP_PORT_STATS: {
        MPPortStatsRequestBuilder stats;
        io.mapRequired("body", stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_QUEUE: {
        MPQueueStatsRequestBuilder stats;
        io.mapRequired("body", stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_METER_CONFIG: {
        MPMeterConfigRequestBuilder stats;
        io.mapRequired("body", stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_METER: {
        MPMeterStatsRequestBuilder stats;
        io.mapRequired("body", stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_GROUP: {
        MPGroupStatsRequestBuilder stats;
        io.mapRequired("body", stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_TABLE_FEATURES: {
        ofp::detail::MPReplyBuilderSeq<MPTableFeaturesBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setRequestBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_EXPERIMENTER: {
        MPExperimenterBuilder stats;
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

#endif  // OFP_YAML_YMULTIPARTREQUEST_H_
