// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/multipartreply.h"
#include "ofp/memorychannel.h"
#include "ofp/message.h"
#include "ofp/mpaggregatestatsreply.h"
#include "ofp/mpdesc.h"
#include "ofp/mpexperimenter.h"
#include "ofp/mpflowmonitorreply.h"
#include "ofp/mpflowstatsreply.h"
#include "ofp/mpgroupdesc.h"
#include "ofp/mpgroupfeatures.h"
#include "ofp/mpgroupstats.h"
#include "ofp/mpmeterconfig.h"
#include "ofp/mpmeterfeatures.h"
#include "ofp/mpmeterstats.h"
#include "ofp/mpportstats.h"
#include "ofp/mpqueuestats.h"
#include "ofp/mptablefeatures.h"
#include "ofp/mptablestats.h"
#include "ofp/port.h"
#include "ofp/queuedesc.h"
#include "ofp/tabledesc.h"
#include "ofp/writable.h"

using namespace ofp;

bool MultipartReply::validateInput(Validation *context) const {
  context->setLengthRemaining(replyBodySize());

  switch (replyType()) {
    case OFPMP_DESC:
      return context->validate<MPDesc>(replyBody(), OFP_VERSION_1);
    case OFPMP_FLOW:
      return context->validateArrayVariableSize<MPFlowStatsReply>(
          replyBody(), OFP_VERSION_1);
    case OFPMP_AGGREGATE:
      return context->validate<MPAggregateStatsReply>(replyBody(),
                                                      OFP_VERSION_1);
    case OFPMP_TABLE: {
      // We do NOT support OFPMP_TABLE_STATS for OF versions 1.1 and 1.2.
      UInt8 vers = header_.version();
      if (vers == OFP_VERSION_2 || vers == OFP_VERSION_3) {
        context->multipartTypeIsNotSupportedForVersion();
        return false;
      }
      return context->validateArrayFixedSize<MPTableStats>(replyBody(),
                                                           OFP_VERSION_1);
  }
    case OFPMP_PORT_STATS:
      return context->validateArrayVariableSize<MPPortStats>(replyBody(),
                                                             OFP_VERSION_1);
    case OFPMP_QUEUE:
      return context->validateArrayFixedSize<MPQueueStats>(replyBody(),
                                                           OFP_VERSION_1);
    case OFPMP_GROUP:
      return context->validateArrayVariableSize<MPGroupStats>(replyBody(),
                                                              OFP_VERSION_2);
    case OFPMP_GROUP_DESC:
      return context->validateArrayVariableSize<MPGroupDesc>(replyBody(),
                                                             OFP_VERSION_2);
    case OFPMP_GROUP_FEATURES:
      return context->validate<MPGroupFeatures>(replyBody(), OFP_VERSION_3);
    case OFPMP_METER:
      return context->validateArrayVariableSize<MPMeterStats>(replyBody(),
                                                              OFP_VERSION_4);
    case OFPMP_METER_CONFIG:
      return context->validateArrayVariableSize<MPMeterConfig>(replyBody(),
                                                               OFP_VERSION_4);
    case OFPMP_METER_FEATURES:
      return context->validate<MPMeterFeatures>(replyBody(), OFP_VERSION_4);
    case OFPMP_TABLE_FEATURES:
      return context->validateArrayVariableSize<MPTableFeatures>(replyBody(),
                                                                 OFP_VERSION_4);
    case OFPMP_PORT_DESC:
      return context->validateArrayVariableSize<Port>(replyBody(),
                                                      OFP_VERSION_1);
    case OFPMP_TABLE_DESC:
      return context->validateArrayVariableSize<TableDesc>(replyBody(),
                                                           OFP_VERSION_5);
    case OFPMP_QUEUE_DESC:
      return context->validateArrayVariableSize<QueueDesc>(replyBody(),
                                                           OFP_VERSION_5);
    case OFPMP_FLOW_MONITOR:
      return context->validateArrayVariableSize<MPFlowMonitorReply>(
          replyBody(), OFP_VERSION_5);
    case OFPMP_EXPERIMENTER:
      return context->validate<MPExperimenter>(replyBody(), OFP_VERSION_1,
                                               false);
    case OFPMP_UNSUPPORTED:
      break;
  }

  context->multipartTypeIsNotSupported();

  return false;
}

UInt32 MultipartReplyBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();

  size_t msgLen = body_.size();
  if (version == OFP_VERSION_1) {
    msgLen += MultipartReply::UnpaddedSizeVersion1;
  } else {
    msgLen += sizeof(msg_);
  }

  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);
  msg_.header_.setLength(msgLen);

  if (version == OFP_VERSION_1) {
    msg_.header_.setRawType(deprecated::v1::OFPT_STATS_REPLY);
    channel->write(&msg_, MultipartReply::UnpaddedSizeVersion1);
  } else {
    msg_.header_.setType(OFPT_MULTIPART_REPLY);
    channel->write(&msg_, sizeof(msg_));
  }

  channel->write(body_.data(), body_.size());
  channel->flush();

  return xid;
}

void MultipartReplyBuilder::sendUsingReplyBody(MemoryChannel *channel,
                                               const void *data, size_t length,
                                               size_t offset) {
  // Break reply body into chunks on a clean boundary. Offset specifies where
  // the length is located.

  ByteRange body{data, length};

  while (body.size() > MAX_BODY_SIZE) {
    size_t chunkSize =
        detail::ProtocolRangeSplitOffset(MAX_BODY_SIZE, 0, body, offset);
    assert(chunkSize <= MAX_BODY_SIZE);
    assert(chunkSize <= body.size());
    assert(chunkSize > 0);

    setReplyFlags(OFPMPF_MORE);
    setReplyBody(body.data(), chunkSize);
    send(channel);

    // Keep sending messages with the same xid.
    channel->setNextXid(msg_.header_.xid());

    body = SafeByteRange(body.data(), body.size(), chunkSize);
  }

  setReplyFlags(OFPMPF_NONE);
  setReplyBody(body.data(), body.size());
}
