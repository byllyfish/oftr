// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMULTIPARTREQUEST_H_
#define OFP_YAML_YMULTIPARTREQUEST_H_

#include "ofp/memorychannel.h"
#include "ofp/mpflowstatsrequest.h"
#include "ofp/mpmeterstatsrequest.h"
#include "ofp/multipartrequest.h"
#include "ofp/yaml/ympexperimenter.h"
#include "ofp/yaml/ympflowmonitorrequest.h"
#include "ofp/yaml/ympflowstatsrequest.h"
#include "ofp/yaml/ympgroupstatsrequest.h"
#include "ofp/yaml/ympmeterconfigrequest.h"
#include "ofp/yaml/ympportstatsrequest.h"
#include "ofp/yaml/ympqueuedescrequest.h"
#include "ofp/yaml/ympqueuestatsrequest.h"
#include "ofp/yaml/ympreplyseq.h"
#include "ofp/yaml/ymptablefeatures.h"

namespace llvm {
namespace yaml {

const char *const kMultipartRequestSchema = R"""(
{Message/Request.Desc}
type: REQUEST.DESC

{Message/Request.Table}
type: REQUEST.TABLE

{Message/Request.PortDesc}
type: REQUEST.PORT_DESC

{Message/Request.GroupDesc}
type: REQUEST.GROUP_DESC

{Message/Request.GroupFeatures}
type: REQUEST.GROUP_FEATURES

{Message/Request.MeterFeatures}
type: REQUEST.METER_FEATURES

{Message/Request.Flow}
type: REQUEST.FLOW
msg:
  table_id: TableNumber
  out_port: PortNumber
  out_group: GroupNumber
  cookie: UInt64
  cookie_mask: UInt64
  match: [Field]

{Message/Request.Aggregate}
type: REQUEST.AGGREGATE
msg:
  table_id: TableNumber
  out_port: PortNumber
  out_group: GroupNumber
  cookie: UInt64
  cookie_mask: UInt64
  match: [Field]

{Message/Request.PortStats}
type: REQUEST.PORT_STATS
msg:
  port_no: PortNumber

{Message/Request.Queue}
type: REQUEST.QUEUE
msg:
  port_no: PortNumber
  queue_id: QueueNumber

{Message/Request.QueueDesc}
type: REQUEST.QUEUE_DESC
msg:
  port_no: PortNumber
  queue_id: QueueNumber

{Message/Request.MeterConfig}
type: REQUEST.METER_CONFIG
msg:
  meter_id: MeterNumber

{Message/Request.Meter}
type: REQUEST.METER
msg:
  meter_id: MeterNumber

{Message/Request.Group}
type: REQUEST.GROUP
msg:
  group_id: GroupNumber

{Message/Request.TableFeatures}
type: REQUEST.TABLE_FEATURES
msg:
  - table_id: TableNumber
    name: Str32
    metadata_match: UInt64
    metadata_write: UInt64
    config: TableConfigFlags
    max_entries: UInt32
    instructions: [InstructionID]
    instructions_miss: !optout [InstructionID]
    next_tables: [UInt8]
    next_tables_miss: !optout [UInt8]
    write_actions: [ActionID]
    write_actions_miss: !optout [ActionID]
    apply_actions: [ActionID]
    apply_actions_miss: !optout [ActionID]
    match: [FieldID]
    wildcards: [FieldID]
    write_set_field: [FieldID]
    write_set_field_miss: !optout [FieldID]
    apply_set_field: [FieldID]
    apply_set_field_miss: !optout [FieldID]
    properties: !opt [ExperimenterProperty]

{Message/Request.TableDesc}
type: REQUEST.TABLE_DESC

{Message/Request.FlowMonitor}
type: REQUEST.FLOW_MONITOR
msg:
  monitor_id: UInt32
  out_port: PortNumber
  out_group: GroupNumber
  flags: [FlowMonitorFlags]
  table_id: TableNumber
  command: FlowMonitorCommand
  match: [Field]

{Message/Request.Experimenter}
type: REQUEST.EXPERIMENTER
msg:
  experimenter: UInt32
  exp_type: UInt32
  data: HexData
)""";

struct EmptyObject {};

template <>
struct MappingTraits<ofp::MultipartRequest> {
  static void mapping(IO &io, ofp::MultipartRequest &msg) {
    using namespace ofp;

    OFPMultipartType type = msg.requestType();
    io.mapRequired("type", type);
    io.mapRequired("flags", msg.flags_);

    decode(io, msg, type, "body");
  }

  static void decode(IO &io, ofp::MultipartRequest &msg,
                     ofp::OFPMultipartType type, const char *key) {
    using namespace ofp;

    switch (type) {
      case OFPMP_DESC:
      case OFPMP_TABLE:
      case OFPMP_PORT_DESC:
      case OFPMP_GROUP_DESC:
      case OFPMP_GROUP_FEATURES:
      case OFPMP_METER_FEATURES:
      case OFPMP_TABLE_DESC: {
        EmptyObject empty;
        io.mapRequired(key, empty);
        break;
      }
      case OFPMP_FLOW:
      case OFPMP_AGGREGATE: {
        const MPFlowStatsRequest *stats = MPFlowStatsRequest::cast(&msg);
        if (stats) {
          io.mapRequired(key, RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_PORT_STATS: {
        const MPPortStatsRequest *stats = MPPortStatsRequest::cast(&msg);
        if (stats) {
          io.mapRequired(key, RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_QUEUE: {
        const MPQueueStatsRequest *stats = MPQueueStatsRequest::cast(&msg);
        if (stats) {
          io.mapRequired(key, RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_QUEUE_DESC: {
        const MPQueueDescRequest *req = MPQueueDescRequest::cast(&msg);
        if (req) {
          io.mapRequired(key, RemoveConst_cast(*req));
        }
        break;
      }
      case OFPMP_METER_CONFIG: {
        const MPMeterConfigRequest *stats = MPMeterConfigRequest::cast(&msg);
        if (stats) {
          io.mapRequired(key, RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_METER: {
        const MPMeterStatsRequest *stats = MPMeterStatsRequest::cast(&msg);
        if (stats) {
          io.mapRequired(key, RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_GROUP: {
        const MPGroupStatsRequest *stats = MPGroupStatsRequest::cast(&msg);
        if (stats) {
          io.mapRequired(key, RemoveConst_cast(*stats));
        }
        break;
      }
      case OFPMP_TABLE_FEATURES: {
        ofp::detail::MPReplyVariableSizeSeq<MPTableFeatures> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_FLOW_MONITOR: {
        const MPFlowMonitorRequest *monitor = MPFlowMonitorRequest::cast(&msg);
        if (monitor) {
          io.mapRequired(key, RemoveConst_cast(*monitor));
        }
        break;
      }
      case OFPMP_EXPERIMENTER: {
        const MPExperimenter *stats = MPExperimenter::cast(&msg);
        if (stats) {
          io.mapRequired(key, RemoveConst_cast(*stats));
        }
        break;
      }
      default:
        // FIXME - implement the rest.
        log_debug("MultipartRequest MappingTraits not fully implemented.");
        break;
    }
  }
};

template <>
struct MappingTraits<ofp::MultipartRequestBuilder> {
  static void mapping(IO &io, ofp::MultipartRequestBuilder &msg) {
    using namespace ofp;

    OFPMultipartType type = OFPMP_UNSUPPORTED;
    io.mapRequired("type", type);
    io.mapRequired("flags", msg.msg_.flags_);
    msg.setRequestType(type);

    encode(io, msg, type, "body");
  }

  static void encode(IO &io, ofp::MultipartRequestBuilder &msg,
                     ofp::OFPMultipartType type, const char *key) {
    using namespace ofp;

    switch (type) {
      case OFPMP_DESC:
      case OFPMP_TABLE:
      case OFPMP_PORT_DESC:
      case OFPMP_GROUP_DESC:
      case OFPMP_GROUP_FEATURES:
      case OFPMP_METER_FEATURES:
      case OFPMP_TABLE_DESC: {
        EmptyObject empty;
        io.mapOptional(key, empty);
        break;
      }
      case OFPMP_FLOW:
      case OFPMP_AGGREGATE: {
        MPFlowStatsRequestBuilder stats;
        io.mapRequired(key, stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_PORT_STATS: {
        MPPortStatsRequestBuilder stats;
        io.mapRequired(key, stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_QUEUE: {
        MPQueueStatsRequestBuilder stats;
        io.mapRequired(key, stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_QUEUE_DESC: {
        MPQueueDescRequestBuilder req;
        io.mapRequired(key, req);
        MemoryChannel channel{msg.msg_.header_.version()};
        req.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_METER_CONFIG: {
        MPMeterConfigRequestBuilder stats;
        io.mapRequired(key, stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_METER: {
        MPMeterStatsRequestBuilder stats;
        io.mapRequired(key, stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_GROUP: {
        MPGroupStatsRequestBuilder stats;
        io.mapRequired(key, stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_TABLE_FEATURES: {
        ofp::detail::MPReplyBuilderSeq<MPTableFeaturesBuilder> seq{
            msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        sendMultipleParts(io, msg, seq.data(), seq.size(),
                          MPTableFeatures::MPVariableSizeOffset);
        break;
      }
      case OFPMP_FLOW_MONITOR: {
        MPFlowMonitorRequestBuilder monitor;
        io.mapRequired(key, monitor);
        MemoryChannel channel{msg.msg_.header_.version()};
        monitor.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      case OFPMP_EXPERIMENTER: {
        MPExperimenterBuilder stats;
        io.mapRequired(key, stats);
        MemoryChannel channel{msg.msg_.header_.version()};
        stats.write(&channel);
        msg.setRequestBody(channel.data(), channel.size());
        break;
      }
      default:
        // FIXME - implement the rest.
        log_debug(
            "MultipartRequestBuilder MappingTraits not fully implemented.");
        break;
    }
  }

 private:
  static void sendMultipleParts(IO &io, ofp::MultipartRequestBuilder &msg,
                                const void *data, size_t length,
                                size_t offset) {
    if (length <= ofp::MultipartRequestBuilder::MAX_BODY_SIZE) {
      msg.setRequestBody(data, length);
    } else {
      // Break message into chunks. Note that `sendWithRequestBody` leaves
      // the request body set to the final chunk, and does *not* send it.
      auto encoder = ofp::yaml::GetEncoderFromContext(io);
      if (encoder && !encoder->recursive()) {
        msg.sendUsingRequestBody(encoder->memoryChannel(), data, length,
                                 offset);
      } else {
        log_warning("Recursive multipart request forbidden");
        io.setError("Recursive multipart request forbidden");
      }
    }
  }
};

template <>
struct MappingTraits<llvm::yaml::EmptyObject> {
  static void mapping(IO &io, EmptyObject &obj) {}
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMULTIPARTREQUEST_H_
