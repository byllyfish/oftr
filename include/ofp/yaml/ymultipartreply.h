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

const char *const kMultipartReplySchema = R"""({Message/MultipartReply}
type: 'MULTIPART_REPLY'
msg:
  type: <MultipartType>
  flags: MultipartFlags
  body: <MultipartBody>
)""";

template <>
struct MappingTraits<ofp::MultipartReply> {
  static void mapping(IO &io, ofp::MultipartReply &msg) {
    using namespace ofp;

    OFPMultipartType type = msg.replyType();
    OFPMultipartFlags flags = msg.replyFlags();
    io.mapRequired("type", type);
    io.mapRequired("flags", flags);

    switch (type) {
      case OFPMP_DESC: {
        MPDesc *desc = RemoveConst_cast(msg.body_cast<MPDesc>());
        if (desc) {
          io.mapRequired("body", *desc);
        }
        break;
      }
      case OFPMP_FLOW: {
        ofp::detail::MPReplyVariableSizeSeq<MPFlowStatsReply> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_AGGREGATE: {
        MPAggregateStatsReply *reply =
            RemoveConst_cast(msg.body_cast<MPAggregateStatsReply>());
        if (reply) {
          io.mapRequired("body", *reply);
        }
        break;
      }
      case OFPMP_TABLE: {
        ofp::detail::MPReplyFixedSizeSeq<MPTableStats> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_PORT_STATS: {
        ofp::detail::MPReplyVariableSizeSeq<MPPortStats> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_QUEUE: {
        ofp::detail::MPReplyFixedSizeSeq<MPQueueStats> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_PORT_DESC: {
        ofp::detail::MPReplyVariableSizeSeq<Port> seq{msg};
        io.mapRequired("body", seq);
        // io.mapOptional("body", EmptyRequest);
        break;
      }
      case OFPMP_GROUP_DESC: {
        ofp::detail::MPReplyVariableSizeSeq<MPGroupDesc> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_GROUP_FEATURES: {
        MPGroupFeatures *features =
            RemoveConst_cast(msg.body_cast<MPGroupFeatures>());
        if (features) {
          io.mapRequired("body", *features);
        }
        break;
      }
      case OFPMP_METER_CONFIG: {
        ofp::detail::MPReplyVariableSizeSeq<MPMeterConfig> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_METER: {
        ofp::detail::MPReplyVariableSizeSeq<MPMeterStats> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_METER_FEATURES: {
        MPMeterFeatures *features =
            RemoveConst_cast(msg.body_cast<MPMeterFeatures>());
        if (features) {
          io.mapRequired("body", *features);
        }
        break;
      }
      case OFPMP_TABLE_FEATURES: {
        ofp::detail::MPReplyVariableSizeSeq<MPTableFeatures> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_GROUP: {
        ofp::detail::MPReplyVariableSizeSeq<MPGroupStats> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_FLOW_MONITOR: {
        ofp::detail::MPReplyVariableSizeSeq<MPFlowMonitorReply> seq{msg};
        io.mapRequired("body", seq);
        break;
      }
      case OFPMP_EXPERIMENTER: {
        MPExperimenter *exper =
            RemoveConst_cast(msg.body_cast<MPExperimenter>());
        if (exper) {
          io.mapRequired("body", *exper);
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
    OFPMultipartFlags flags;
    io.mapRequired("type", type);
    io.mapRequired("flags", flags);
    msg.setReplyType(type);
    msg.setReplyFlags(flags);

    switch (type) {
      case OFPMP_DESC: {
        MPDescBuilder desc;
        io.mapRequired("body", desc);
        // FIXME - write reply into channel.
        msg.setReplyBody(&desc, sizeof(desc));
        break;
      }
      case OFPMP_FLOW: {
        ofp::detail::MPReplyBuilderSeq<MPFlowStatsReplyBuilder> seq{
            msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_AGGREGATE: {
        MPAggregateStatsReplyBuilder reply;
        io.mapRequired("body", reply);
        // FIXME - write reply into channel.
        msg.setReplyBody(&reply, sizeof(reply));
        break;
      }
      case OFPMP_TABLE: {
        ofp::detail::MPReplyBuilderSeq<MPTableStatsBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_PORT_STATS: {
        ofp::detail::MPReplyBuilderSeq<MPPortStatsBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_QUEUE: {
        ofp::detail::MPReplyBuilderSeq<MPQueueStatsBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_PORT_DESC: {
        ofp::detail::MPReplyBuilderSeq<PortBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_GROUP_DESC: {
        ofp::detail::MPReplyBuilderSeq<MPGroupDescBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_GROUP_FEATURES: {
        MPGroupFeaturesBuilder features;
        io.mapRequired("body", features);
        // FIXME - write reply into channel.
        msg.setReplyBody(&features, sizeof(features));
        break;
      }
      case OFPMP_METER_CONFIG: {
        ofp::detail::MPReplyBuilderSeq<MPMeterConfigBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_METER: {
        ofp::detail::MPReplyBuilderSeq<MPMeterStatsBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_METER_FEATURES: {
        MPMeterFeaturesBuilder features;
        io.mapRequired("body", features);
        // FIXME - write reply into channel.
        msg.setReplyBody(&features, sizeof(features));
        break;
      }
      case OFPMP_TABLE_FEATURES: {
        ofp::detail::MPReplyBuilderSeq<MPTableFeaturesBuilder> seq{
            msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_GROUP: {
        ofp::detail::MPReplyBuilderSeq<MPGroupStatsBuilder> seq{msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_FLOW_MONITOR: {
        ofp::detail::MPReplyBuilderSeq<MPFlowMonitorReplyBuilder> seq{
            msg.version()};
        io.mapRequired("body", seq);
        seq.close();
        msg.setReplyBody(seq.data(), seq.size());
        break;
      }
      case OFPMP_EXPERIMENTER: {
        MPExperimenterBuilder exper;
        io.mapRequired("body", exper);
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
