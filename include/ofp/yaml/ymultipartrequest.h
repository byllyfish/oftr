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

const char *const kMultipartRequestSchema = R"""(
{Message/Request.Desc}
type: 'REQUEST.DESC'

{Message/Request.Table}
type: 'REQUEST.TABLE'

{Message/Request.PortDesc}
type: 'REQUEST.PORT_DESC'

{Message/Request.GroupDesc}
type: 'REQUEST.GROUP_DESC'

{Message/Request.GroupFeatures}
type: 'REQUEST.GROUP_FEATURES'

{Message/Request.MeterFeatures}
type: 'REQUEST.METER_FEATURES'

{Message/Request.Flow}
type: 'REQUEST.FLOW'
msg:
  table_id: TableNumber
  out_port: PortNumber
  out_group: GroupNumber
  cookie: UInt64
  cookie_mask: UInt64
  match: [{Field}]

{Message/Request.Aggregate}
type: 'REQUEST.AGGREGATE'
msg:
  table_id: TableNumber
  out_port: PortNumber
  out_group: GroupNumber
  cookie: UInt64
  cookie_mask: UInt64
  match: [{Field}]

{Message/Request.PortStats}
type: 'REQUEST.PORT_STATS'
msg:
  port_no: PortNumber

{Message/Request.Queue}
type: 'REQUEST.QUEUE'
msg:
  port_no: PortNumber
  queue_id: UInt32

{Message/Request.MeterConfig}
type: 'REQUEST.METER_CONFIG'
msg:
  meter_id: UInt32

{Message/Request.Meter}
type: 'REQUEST.METER'
msg:
  meter_id: UInt32

{Message/Request.Group}
type: 'REQUEST.GROUP'
msg:
  group_id: UInt32

{Message/Request.TableFeatures}
type: 'REQUEST.TABLE_FEATURES'
msg:
  - table_id: TableNumber
    name: Str31
    metadata_match: UInt64
    metadata_write: UInt64
    config: UInt32
    max_entries: UInt32
    instructions: [InstructionID]
    instructions_miss: [InstructionID]     # Optional; Default=[]
    next_tables: [TableID]
    next_tables_miss: [TableID]            # Optional; Default=[]
    write_actions: [ActionID]
    write_actions_miss: [ActionID]         # Optional; Default=[]
    apply_actions: [ActionID]
    apply_actions_miss: [ActionID]         # Optional; Default=[]
    match: [FieldID]
    wildcards: [FieldID]
    write_set_field: [FieldID]
    write_set_field_miss: [FieldID]        # Optional; Default=[]
    apply_set_field: [FieldID]
    apply_set_field_miss: [FieldID]        # Optional; Default=[]
    properties:

{Message/Request.FlowMonitor}
type: 'REQUEST.FLOW_MONITOR'
msg:
  monitor_id: UInt32
  out_port: PortNumber
  out_group: UInt32
  flags: UInt16
  table_id: TableNumber
  command: UInt8
  match: [{Field}]

{Message/Request.Experimenter}
type: 'REQUEST.EXPERIMENTER'
msg:
  experimenter: UInt32
  exp_type: UInt32 
  data: HexString
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
      case OFPMP_METER_FEATURES: {
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
        log::debug("MultipartRequest MappingTraits not fully implemented.");
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
      case OFPMP_METER_FEATURES: {
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
        msg.setRequestBody(seq.data(), seq.size());
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
        log::debug(
            "MultipartRequestBuilder MappingTraits not fully implemented.");
        break;
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
