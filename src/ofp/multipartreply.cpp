//  ===== ---- ofp/multipartreply.cpp ----------------------*- C++ -*- =====  //
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
/// \brief Implements MultipartReply class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/multipartreply.h"
#include "ofp/writable.h"
#include "ofp/message.h"
#include "ofp/mpdesc.h"
#include "ofp/mptablestats.h"
#include "ofp/mpgroupdesc.h"
#include "ofp/mpgroupfeatures.h"
#include "ofp/mpmeterfeatures.h"
#include "ofp/port.h"
#include "ofp/mpflowstatsreply.h"
#include "ofp/mpaggregatestatsreply.h"
#include "ofp/mpportstats.h"
#include "ofp/mpqueuestats.h"
#include "ofp/mpgroupstats.h"
#include "ofp/mpmeterstats.h"
#include "ofp/mpmeterconfig.h"
#include "ofp/mptablefeatures.h"
#include "ofp/mpexperimenter.h"

using namespace ofp;



bool MultipartReply::validateInput(Validation *context) const {
  context->setLengthRemaining(replyBodySize());

  switch (replyType()) {
    case OFPMP_DESC:
      return context->validate<MPDesc>(replyBody(), OFP_VERSION_1);
    case OFPMP_FLOW:
      return context->validateArrayVariableSize<MPFlowStatsReply>(replyBody(), OFP_VERSION_1);
    case OFPMP_AGGREGATE:
      return context->validate<MPAggregateStatsReply>(replyBody(), OFP_VERSION_1);
    case OFPMP_TABLE:
      return context->validateArrayFixedSize<MPTableStats>(replyBody(), OFP_VERSION_1);
    case OFPMP_PORT_STATS:
      return context->validateArrayFixedSize<MPPortStats>(replyBody(), OFP_VERSION_1);
    case OFPMP_QUEUE:
      return context->validateArrayFixedSize<MPQueueStats>(replyBody(), OFP_VERSION_1);
    case OFPMP_GROUP:
      return context->validateArrayVariableSize<MPGroupStats>(replyBody(), OFP_VERSION_2);
    case OFPMP_GROUP_DESC:
      return context->validateArrayVariableSize<MPGroupDesc>(replyBody(), OFP_VERSION_2);
    case OFPMP_GROUP_FEATURES:
      return context->validate<MPGroupFeatures>(replyBody(), OFP_VERSION_3);
    case OFPMP_METER:
      return context->validateArrayVariableSize<MPMeterStats>(replyBody(), OFP_VERSION_4);
    case OFPMP_METER_CONFIG:
      return context->validateArrayVariableSize<MPMeterConfig>(replyBody(), OFP_VERSION_4);
    case OFPMP_METER_FEATURES:
      return context->validate<MPMeterFeatures>(replyBody(), OFP_VERSION_4);
    case OFPMP_TABLE_FEATURES:
      return context->validateArrayVariableSize<MPTableFeatures>(replyBody(), OFP_VERSION_4);
    case OFPMP_PORT_DESC:
      return context->validateArrayFixedSize<Port>(replyBody(), OFP_VERSION_4);
    case OFPMP_EXPERIMENTER:
      return context->validate<MPExperimenter>(replyBody(), OFP_VERSION_1);
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
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  if (version == OFP_VERSION_1) {
    msg_.header_.setType(deprecated::v1::OFPT_STATS_REPLY);
    channel->write(&msg_, MultipartReply::UnpaddedSizeVersion1);
  } else {
    msg_.header_.setType(OFPT_MULTIPART_REPLY);
    channel->write(&msg_, sizeof(msg_));
  }

  channel->write(body_.data(), body_.size());
  channel->flush();

  return xid;
}

