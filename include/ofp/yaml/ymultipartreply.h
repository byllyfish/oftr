// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YMULTIPARTREPLY_H_
#define OFP_YAML_YMULTIPARTREPLY_H_

#include "ofp/multipartreply.h"
#include "ofp/yaml/ympflowstatsreply.h"
#include "ofp/yaml/ympdesc.h"
#include "ofp/yaml/ympaggregatestatsreply.h"
#include "ofp/yaml/ymptablestats.h"
#include "ofp/yaml/ympportstats.h"
#include "ofp/yaml/ympqueuestats.h"
#include "ofp/yaml/ympgroupdesc.h"
#include "ofp/yaml/ympgroupfeatures.h"
#include "ofp/yaml/ympmeterconfig.h"
#include "ofp/yaml/ympmeterstats.h"
#include "ofp/yaml/ympmeterfeatures.h"
#include "ofp/yaml/ymptablefeatures.h"
#include "ofp/yaml/ympgroupstats.h"
#include "ofp/yaml/ympflowmonitorreply.h"
#include "ofp/yaml/ympexperimenter.h"
#include "ofp/yaml/ympreplyseq.h"

namespace llvm {
namespace yaml {

const char *const kMultipartReplySchema = R"""(
{Message/Reply.Desc}
type: 'REPLY.DESC'
msg:
  mfr_desc: Str256
  hw_desc: Str256
  sw_desc: Str256
  serial_num: Str32
  dp_desc: Str256

{Message/Reply.Table}
type: 'REPLY.TABLE'
msg:
  - table_id: UInt8
    name: Str32             # V1
    wildcards: UInt32
    max_entries: UInt32
    active_count: UInt32
    lookup_count: UInt64
    matched_count: UInt64

{Message/Reply.PortDesc}
type: 'REPLY.PORT_DESC'
msg:
  - {Port}

{Message/Reply.GroupDesc}
type: 'REPLY.GROUP_DESC'
msg:
  - type: UInt8
    group_id: UInt32
    buckets:
      - {Bucket}

{Message/Reply.GroupFeatures}
type: 'REPLY.GROUP_FEATURES'
msg:
  types: UInt32
  capabilities: UInt32
  max_groups_all: [ActionTypeFlag]
  max_groups_sel: [ActionTypeFlag]
  max_groups_ind: [ActionTypeFlag]
  max_groups_ff: [ActionTypeFlag]
  actions_groups_all: [ActionTypeFlag]
  actions_groups_sel: [ActionTypeFlag]
  actions_groups_ind: [ActionTypeFlag]
  actions_groups_ff: [ActionTypeFlag]

{Message/Reply.MeterFeatures}
type: 'REPLY.METER_FEATURES'
msg:
  max_meter: UInt32
  band_types: UInt32
  capabilities: UInt32
  max_bands: UInt8
  max_color: UInt8

{Message/Reply.Flow}
type: 'REPLY.FLOW'
msg:
  table_id: UInt8
  duration_sec: UInt32
  duration_nsec: UInt32
  priority: UInt16
  idle_timeout: UInt16
  hard_timeout: UInt16
  flags: [FlowModFlag]
  cookie: UInt64
  packet_count: UInt64
  byte_count: UInt64
  match: [{Field}]
  instructions: [{Instruction}]

{Message/Reply.Aggregate}
type: 'REPLY.AGGREGATE'
msg:
  packet_count: UInt64
  byte_count: UInt64
  flow_count: UInt64

{Message/Reply.PortStats}
type: 'REPLY.PORT_STATS'
msg:
  - port_no: PortNumber
    duration_sec: UInt32
    duration_nsec: UInt32
    rx_packets: UInt64
    tx_packets: UInt64
    rx_bytes: UInt64
    tx_bytes: UInt64
    rx_dropped: UInt64
    tx_dropped: UInt64
    rx_errors: UInt64
    tx_errors: UInt64
    ethernet:
      rx_frame_err: UInt64
      rx_over_err: UInt64
      rx_crc_err: UInt64
      collisions: UInt64
    optical:                  # Optional
      flags: UInt32
      tx_freq_lmda: UInt32
      tx_offset: UInt32
      tx_grid_span: UInt32
      rx_freq_lmda: UInt32
      rx_offset: UInt32
      rx_grid_span: UInt32
      tx_pwr: UInt16
      rx_pwr: UInt16
      bias_current: UInt16
      temperature: UInt16

{Message/Reply.Queue}
type: 'REPLY.QUEUE'
msg:
  - port_no: PortNumber
    queue_id: UInt32
    tx_packets: UInt64
    tx_bytes: UInt64
    tx_errors: UInt64
    duration_sec: UInt32
    duration_nsec: UInt32

{Message/Reply.MeterConfig}
type: 'REPLY.METER_CONFIG'
msg:
  - flags: UInt16
    meter_id: UInt32
    bands: [{MeterBand}]

{Message/Reply.Meter}
type: 'REPLY.METER'
msg:
  - meter_id: UInt32
    flow_count: UInt32
    packet_in_count: UInt64
    bytes_in_count: UInt64
    duration_sec: UInt32
    duration_nsec: UInt32
    bands: [{PacketCounter}]

{Message/Reply.Group}
type: 'REPLY.GROUP'
msg:
  - group_id: UInt32
    ref_count: UInt32
    packet_count: UInt64
    byte_count: UInt64
    duration_sec: UInt32
    duration_nsec: UInt32
    bucket_stats: [{PacketCounter}]

{Message/Reply.TableFeatures}
type: 'REPLY.TABLE_FEATURES'
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

{Message/Reply.FlowMonitor}
type: 'REPLY.FLOW_MONITOR'
msg:
  - event: 'INITIAL' | 'ADDED' | 'REMOVED' | 'MODIFIED'
    table_id: UInt8
    reason: UInt8
    idle_timeout: UInt16
    hard_timeout: UInt16
    priority: UInt16
    cookie: UInt64
    match: [{Field}]
    instructions: [{Instruction}]
  - event: 'ABBREV' | 'PAUSED' | 'RESUMED'
    xid: UInt32

{Message/Reply.Experimenter}
type: 'REPLY.EXPERIMENTER'
msg:
  experimenter: UInt32
  exp_type: UInt32 
  data: HexString
)""";

template <>
struct MappingTraits<ofp::MultipartReply> {
  static void mapping(IO &io, ofp::MultipartReply &msg) {
    using namespace ofp;

    OFPMultipartType type = msg.replyType();
    io.mapRequired("type", type);
    io.mapRequired("flags", msg.flags_);

    decode(io, msg, type, "body");
  }

  static void decode(IO &io, ofp::MultipartReply &msg,
                     ofp::OFPMultipartType type, const char *key) {
    using namespace ofp;

    switch (type) {
      case OFPMP_DESC: {
        MPDesc *desc = RemoveConst_cast(msg.body_cast<MPDesc>());
        if (desc) {
          io.mapRequired(key, *desc);
        }
        break;
      }
      case OFPMP_FLOW: {
        ofp::detail::MPReplyVariableSizeSeq<MPFlowStatsReply> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_AGGREGATE: {
        MPAggregateStatsReply *reply =
            RemoveConst_cast(msg.body_cast<MPAggregateStatsReply>());
        if (reply) {
          io.mapRequired(key, *reply);
        }
        break;
      }
      case OFPMP_TABLE: {
        ofp::detail::MPReplyFixedSizeSeq<MPTableStats> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_PORT_STATS: {
        ofp::detail::MPReplyVariableSizeSeq<MPPortStats> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_QUEUE: {
        ofp::detail::MPReplyFixedSizeSeq<MPQueueStats> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_PORT_DESC: {
        ofp::detail::MPReplyVariableSizeSeq<Port> seq{msg};
        io.mapRequired(key, seq);
        // io.mapOptional(key, EmptyRequest);
        break;
      }
      case OFPMP_GROUP_DESC: {
        ofp::detail::MPReplyVariableSizeSeq<MPGroupDesc> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_GROUP_FEATURES: {
        MPGroupFeatures *features =
            RemoveConst_cast(msg.body_cast<MPGroupFeatures>());
        if (features) {
          io.mapRequired(key, *features);
        }
        break;
      }
      case OFPMP_METER_CONFIG: {
        ofp::detail::MPReplyVariableSizeSeq<MPMeterConfig> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_METER: {
        ofp::detail::MPReplyVariableSizeSeq<MPMeterStats> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_METER_FEATURES: {
        MPMeterFeatures *features =
            RemoveConst_cast(msg.body_cast<MPMeterFeatures>());
        if (features) {
          io.mapRequired(key, *features);
        }
        break;
      }
      case OFPMP_TABLE_FEATURES: {
        ofp::detail::MPReplyVariableSizeSeq<MPTableFeatures> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_GROUP: {
        ofp::detail::MPReplyVariableSizeSeq<MPGroupStats> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_FLOW_MONITOR: {
        ofp::detail::MPReplyVariableSizeSeq<MPFlowMonitorReply> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_EXPERIMENTER: {
        MPExperimenter *exper =
            RemoveConst_cast(msg.body_cast<MPExperimenter>());
        if (exper) {
          io.mapRequired(key, *exper);
        }
        break;
      }
      default:
        // FIXME
        log::info("MultiPartReply: MappingTraits not fully implemented.",
                  static_cast<int>(type));
        break;
    }
  }
};

template <>
struct MappingTraits<ofp::MultipartReplyBuilder> {
  static void mapping(IO &io, ofp::MultipartReplyBuilder &msg) {
    using namespace ofp;

    OFPMultipartType type;
    io.mapRequired("type", type);
    io.mapRequired("flags", msg.msg_.flags_);
    msg.setReplyType(type);

    encode(io, msg, type, "body");
  }

  static void encode(IO &io, ofp::MultipartReplyBuilder &msg,
                     ofp::OFPMultipartType type, const char *key) {
    using namespace ofp;

    switch (type) {
      case OFPMP_DESC: {
        MPDescBuilder desc;
        io.mapRequired(key, desc);
        // FIXME - write reply into channel.
        msg.setReplyBody(&desc, sizeof(desc));
        break;
      }
      case OFPMP_FLOW: {
        ofp::detail::MPReplyBuilderSeq<MPFlowStatsReplyBuilder> seq{
            msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_AGGREGATE: {
        MPAggregateStatsReplyBuilder reply;
        io.mapRequired(key, reply);
        // FIXME - write reply into channel.
        msg.setReplyBody(&reply, sizeof(reply));
        break;
      }
      case OFPMP_TABLE: {
        ofp::detail::MPReplyBuilderSeq<MPTableStatsBuilder> seq{msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_PORT_STATS: {
        ofp::detail::MPReplyBuilderSeq<MPPortStatsBuilder> seq{msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_QUEUE: {
        ofp::detail::MPReplyBuilderSeq<MPQueueStatsBuilder> seq{msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_PORT_DESC: {
        ofp::detail::MPReplyBuilderSeq<PortBuilder> seq{msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_GROUP_DESC: {
        ofp::detail::MPReplyBuilderSeq<MPGroupDescBuilder> seq{msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_GROUP_FEATURES: {
        MPGroupFeaturesBuilder features;
        io.mapRequired(key, features);
        // FIXME - write reply into channel.
        msg.setReplyBody(&features, sizeof(features));
        break;
      }
      case OFPMP_METER_CONFIG: {
        ofp::detail::MPReplyBuilderSeq<MPMeterConfigBuilder> seq{msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_METER: {
        ofp::detail::MPReplyBuilderSeq<MPMeterStatsBuilder> seq{msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_METER_FEATURES: {
        MPMeterFeaturesBuilder features;
        io.mapRequired(key, features);
        // FIXME - write reply into channel.
        msg.setReplyBody(&features, sizeof(features));
        break;
      }
      case OFPMP_TABLE_FEATURES: {
        ofp::detail::MPReplyBuilderSeq<MPTableFeaturesBuilder> seq{
            msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_GROUP: {
        ofp::detail::MPReplyBuilderSeq<MPGroupStatsBuilder> seq{msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_FLOW_MONITOR: {
        ofp::detail::MPReplyBuilderSeq<MPFlowMonitorReplyBuilder> seq{
            msg.version()};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_EXPERIMENTER: {
        MPExperimenterBuilder exper;
        io.mapRequired(key, exper);
        MemoryChannel channel{msg.version()};
        exper.write(&channel);
        msg.setReplyBody(channel.data(), channel.size());
        break;
      }
      default:
        // FIXME
        log::info("MultiPartReplyBuilder: MappingTraits not fully implemented.",
                  static_cast<int>(type));
        break;
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMULTIPARTREPLY_H_
