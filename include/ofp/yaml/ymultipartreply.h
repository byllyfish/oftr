// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMULTIPARTREPLY_H_
#define OFP_YAML_YMULTIPARTREPLY_H_

#include "ofp/multipartreply.h"
#include "ofp/yaml/ympaggregatestatsreply.h"
#include "ofp/yaml/ympdesc.h"
#include "ofp/yaml/ympexperimenter.h"
#include "ofp/yaml/ympflowmonitorreply.h"
#include "ofp/yaml/ympflowstatsreply.h"
#include "ofp/yaml/ympgroupdesc.h"
#include "ofp/yaml/ympgroupfeatures.h"
#include "ofp/yaml/ympgroupstats.h"
#include "ofp/yaml/ympmeterconfig.h"
#include "ofp/yaml/ympmeterfeatures.h"
#include "ofp/yaml/ympmeterstats.h"
#include "ofp/yaml/ympportstats.h"
#include "ofp/yaml/ympqueuestats.h"
#include "ofp/yaml/ympreplyseq.h"
#include "ofp/yaml/ymptablefeatures.h"
#include "ofp/yaml/ymptablestats.h"
#include "ofp/yaml/yqueuedesc.h"
#include "ofp/yaml/ytabledesc.h"

namespace llvm {
namespace yaml {

const char *const kMultipartReplySchema = R"""(
{Message/Reply.Desc}
type: REPLY.DESC
msg:
  mfr_desc: Str256
  hw_desc: Str256
  sw_desc: Str256
  serial_num: Str32
  dp_desc: Str256

{Message/Reply.TableStats}
type: REPLY.TABLE_STATS
msg:
  - table_id: TableNumber
    name: Str32
    wildcards: UInt32
    max_entries: UInt32
    active_count: UInt32
    lookup_count: UInt64
    matched_count: UInt64

{Message/Reply.PortDesc}
type: REPLY.PORT_DESC
msg: [Port]

{Message/Reply.GroupDesc}
type: REPLY.GROUP_DESC
msg:
  - type: GroupType
    group_id: GroupNumber
    buckets: [Bucket]

{Message/Reply.GroupFeatures}
type: REPLY.GROUP_FEATURES
msg:
  types: GroupTypeFlags
  capabilities: GroupCapabilityFlags
  max_groups_all: [ActionTypeFlags]
  max_groups_sel: [ActionTypeFlags]
  max_groups_ind: [ActionTypeFlags]
  max_groups_ff: [ActionTypeFlags]
  actions_groups_all: [ActionTypeFlags]
  actions_groups_sel: [ActionTypeFlags]
  actions_groups_ind: [ActionTypeFlags]
  actions_groups_ff: [ActionTypeFlags]

{Message/Reply.MeterFeatures}
type: REPLY.METER_FEATURES
msg:
  max_meter: UInt32
  band_types: MeterBandFlags
  capabilities: MeterFlags
  max_bands: UInt8
  max_color: UInt8

{Message/Reply.FlowDesc}
type: REPLY.FLOW_DESC
msg:
  - table_id: TableNumber
    duration: DurationSec
    priority: UInt16
    idle_timeout: UInt16
    hard_timeout: UInt16
    flags: [FlowModFlags]
    cookie: UInt64
    packet_count: UInt64
    byte_count: UInt64
    match: [Field]
    instructions: [Instruction]

{Message/Reply.Aggregate}
type: REPLY.AGGREGATE_STATS
msg:
  packet_count: UInt64
  byte_count: UInt64
  flow_count: UInt64

{Message/Reply.PortStats}
type: REPLY.PORT_STATS
msg:
  - port_no: PortNumber
    duration: DurationSec
    rx_packets: UInt64
    tx_packets: UInt64
    rx_bytes: UInt64
    tx_bytes: UInt64
    rx_dropped: UInt64
    tx_dropped: UInt64
    rx_errors: UInt64
    tx_errors: UInt64
    rx_frame_err: UInt64
    rx_over_err: UInt64
    rx_crc_err: UInt64
    collisions: UInt64
    optical: !optout
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

{Message/Reply.QueueStats}
type: REPLY.QUEUE
msg:
  - port_no: PortNumber
    queue_id: QueueNumber
    tx_packets: UInt64
    tx_bytes: UInt64
    tx_errors: UInt64
    duration: DurationSec

{Message/Reply.QueueDesc}
type: REPLY.QUEUE_DESC
msg:
  - port_no: PortNumber
    queue_id: QueueNumber
    min_rate: UInt16
    max_rate: UInt16
    properties: !opt [ExperimenterProperty]

{Message/Reply.MeterConfig}
type: REPLY.METER_CONFIG
msg:
  - flags: UInt16
    meter_id: MeterNumber
    bands: [MeterBand]

{Message/Reply.MeterStats}
type: REPLY.METER_STATS
msg:
  - meter_id: MeterNumber
    flow_count: UInt32
    packet_in_count: UInt64
    bytes_in_count: UInt64
    duration: DurationSec
    bands: [PacketCounter]

{Message/Reply.GroupStats}
type: REPLY.GROUP_STATS
msg:
  - group_id: GroupNumber
    ref_count: UInt32
    packet_count: UInt64
    byte_count: UInt64
    duration: DurationSec
    bucket_stats: [PacketCounter]

{Message/Reply.TableFeatures}
type: REPLY.TABLE_FEATURES
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

{Message/Reply.TableDesc}
type: REPLY.TABLE_DESC
msg:
  - table_id: TableNumber
    config: [TableConfigFlags]
    eviction: !optout
      flags: UInt32
    vacancy: !optout
      vacancy_down: UInt8
      vacancy_up: UInt8
      vacancy: UInt8
    properties: !opt [ExperimenterProperty]

{Message/Reply.FlowMonitor}
type: REPLY.FLOW_MONITOR
msg:
  - event: INITIAL | ADDED | REMOVED | MODIFIED
    table_id: TableNumber
    reason: UInt8
    idle_timeout: UInt16
    hard_timeout: UInt16
    priority: UInt16
    cookie: UInt64
    match: [Field]
    instructions: [Instruction]
  - event: ABBREV | PAUSED | RESUMED
    xid: UInt32

{Message/Reply.Experimenter}
type: REPLY.EXPERIMENTER
msg:
  experimenter: UInt32
  exp_type: UInt32 
  data: HexData
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

    const UInt8 version = msg.version();

    switch (type) {
      case OFPMP_DESC: {
        MPDesc *desc = RemoveConst_cast(msg.body_cast<MPDesc>());
        if (desc) {
          io.mapRequired(key, *desc);
        }
        break;
      }
      case OFPMP_FLOW_DESC: {
        ofp::detail::MPReplyVariableSizeSeq<MPFlowStatsReply> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_AGGREGATE_STATS: {
        if (version >= OFP_VERSION_6) {
          MPAggregateStatsReplyV6 *reply =
              RemoveConst_cast(msg.body_cast<MPAggregateStatsReplyV6>());
          if (reply) {
            io.mapRequired(key, *reply);
          }
        } else {
          MPAggregateStatsReply *reply =
              RemoveConst_cast(msg.body_cast<MPAggregateStatsReply>());
          if (reply) {
            io.mapRequired(key, *reply);
          }
        }
        break;
      }
      case OFPMP_TABLE_STATS: {
        ofp::detail::MPReplyFixedSizeSeq<MPTableStats> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_PORT_STATS: {
        ofp::detail::MPReplyVariableSizeSeq<MPPortStats> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_QUEUE_STATS: {
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
      case OFPMP_METER_STATS: {
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
      case OFPMP_TABLE_DESC: {
        ofp::detail::MPReplyVariableSizeSeq<TableDesc> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_QUEUE_DESC: {
        ofp::detail::MPReplyVariableSizeSeq<QueueDesc> seq{msg};
        io.mapRequired(key, seq);
        break;
      }
      case OFPMP_GROUP_STATS: {
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
        log_info("MultiPartReply: MappingTraits not fully implemented.",
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

    const UInt8 version = msg.version();

    switch (type) {
      case OFPMP_DESC: {
        MPDescBuilder desc;
        io.mapRequired(key, desc);
        // FIXME - write reply into channel.
        msg.setReplyBody(&desc, sizeof(desc));
        break;
      }
      case OFPMP_FLOW_DESC: {
        ofp::detail::MPReplyBuilderSeq<MPFlowStatsReplyBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        sendMultipleParts(io, msg, seq.data(), seq.size(),
                          MPFlowStatsReply::MPVariableSizeOffset);
        break;
      }
      case OFPMP_AGGREGATE_STATS: {
        if (version >= OFP_VERSION_6) {
          MPAggregateStatsReplyV6Builder reply6;
          io.mapRequired(key, reply6);
          MemoryChannel channel{version};
          reply6.write(&channel);
          msg.setReplyBody(channel.data(), channel.size());
        } else {
          MPAggregateStatsReplyBuilder reply;
          io.mapRequired(key, reply);
          // FIXME - write reply into channel.
          msg.setReplyBody(&reply, sizeof(reply));
        }
        break;
      }
      case OFPMP_TABLE_STATS: {
        ofp::detail::MPReplyBuilderSeq<MPTableStatsBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        sendMultipleParts(io, msg, seq.data(), seq.size(),
                          PROTOCOL_ITERATOR_SIZE_FIXED);
        break;
      }
      case OFPMP_PORT_STATS: {
        ofp::detail::MPReplyBuilderSeq<MPPortStatsBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        sendMultipleParts(io, msg, seq.data(), seq.size(),
                          MPPortStats::MPVariableSizeOffset);
        break;
      }
      case OFPMP_QUEUE_STATS: {
        ofp::detail::MPReplyBuilderSeq<MPQueueStatsBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        sendMultipleParts(io, msg, seq.data(), seq.size(),
                          PROTOCOL_ITERATOR_SIZE_FIXED);
        break;
      }
      case OFPMP_PORT_DESC: {
        ofp::detail::MPReplyBuilderSeq<PortBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        sendMultipleParts(io, msg, seq.data(), seq.size(),
                          Port::MPVariableSizeOffset);
        break;
      }
      case OFPMP_GROUP_DESC: {
        ofp::detail::MPReplyBuilderSeq<MPGroupDescBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        sendMultipleParts(io, msg, seq.data(), seq.size(),
                          MPGroupDesc::MPVariableSizeOffset);
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
        ofp::detail::MPReplyBuilderSeq<MPMeterConfigBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        sendMultipleParts(io, msg, seq.data(), seq.size(),
                          MPMeterConfig::MPVariableSizeOffset);
        break;
      }
      case OFPMP_METER_STATS: {
        ofp::detail::MPReplyBuilderSeq<MPMeterStatsBuilder> seq{version};
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
        ofp::detail::MPReplyBuilderSeq<MPTableFeaturesBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        sendMultipleParts(io, msg, seq.data(), seq.size(),
                          MPTableFeatures::MPVariableSizeOffset);
        break;
      }
      case OFPMP_TABLE_DESC: {
        ofp::detail::MPReplyBuilderSeq<TableDescBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_QUEUE_DESC: {
        ofp::detail::MPReplyBuilderSeq<QueueDescBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_GROUP_STATS: {
        ofp::detail::MPReplyBuilderSeq<MPGroupStatsBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_FLOW_MONITOR: {
        ofp::detail::MPReplyBuilderSeq<MPFlowMonitorReplyBuilder> seq{version};
        io.mapRequired(key, seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_EXPERIMENTER: {
        MPExperimenterBuilder exper;
        io.mapRequired(key, exper);
        MemoryChannel channel{version};
        exper.write(&channel);
        msg.setReplyBody(channel.data(), channel.size());
        break;
      }
      default:
        // FIXME
        log_info("MultiPartReplyBuilder: MappingTraits not fully implemented.",
                 static_cast<int>(type));
        break;
    }
  }

 private:
  static void sendMultipleParts(IO &io, ofp::MultipartReplyBuilder &msg,
                                const void *data, size_t length,
                                size_t offset) {
    if (length <= ofp::MultipartReplyBuilder::MAX_BODY_SIZE) {
      msg.setReplyBody(data, length);
    } else {
      // Break message into chunks. Note that `sendWithReplyBody` leaves
      // the reply body set to the final chunk, and does *not* send it.
      auto encoder = ofp::yaml::GetEncoderFromContext(io);
      if (encoder && !encoder->recursive()) {
        msg.sendUsingReplyBody(encoder->memoryChannel(), data, length, offset);
      } else {
        log_warning("Recursive multipart reply forbidden");
        io.setError("Recursive multipart reply forbidden");
      }
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMULTIPARTREPLY_H_
