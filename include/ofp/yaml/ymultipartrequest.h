// Copyright 2014-present Bill Fisher. All rights reserved.

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
#include "ofp/yaml/ympflowmonitorrequest.h"
#include "ofp/yaml/ympexperimenter.h"
#include "ofp/yaml/ympreplyseq.h"
#include "ofp/mpmeterstatsrequest.h"
#include "ofp/memorychannel.h"

namespace llvm {
namespace yaml {

const char *const kMultipartRequestSchema = R"""({Message/MultipartRequest}
type: 'MULTIPART_REQUEST'
msg:
  type: <MultipartType>
  flags: MultipartFlags
  body: <MultipartBody>
)""";

template <>
struct MappingTraits<ofp::MultipartRequest> {
  static void mapping(IO &io, ofp::MultipartRequest &msg) {
    using namespace ofp;

    OFPMultipartType type = msg.requestType();
    io.mapRequired("type", type);
    OFPMultipartFlags flags = msg.requestFlags();
    io.mapRequired("flags", flags);

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
      case OFPMP_FLOW_MONITOR: {
        const MPFlowMonitorRequest *monitor = MPFlowMonitorRequest::cast(&msg);
        if (monitor) {
          io.mapRequired("body", RemoveConst_cast(*monitor));
        }
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
    OFPMultipartFlags flags;
    io.mapRequired("type", type);
    io.mapRequired("flags", flags);
    msg.setRequestType(type);
    msg.setRequestFlags(flags);

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
        ofp::detail::MPReplyBuilderSeq<MPTableFeaturesBuilder> seq{
            msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setRequestBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_FLOW_MONITOR: {
        MPFlowMonitorRequestBuilder monitor;
        io.mapRequired("body", monitor);
        MemoryChannel channel{msg.msg_.header_.version()};
        monitor.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
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
