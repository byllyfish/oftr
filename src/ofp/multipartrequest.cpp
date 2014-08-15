//  ===== ---- ofp/multipartrequest.cpp --------------------*- C++ -*- =====  //
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
/// \brief Implements MultipartRequest class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/multipartrequest.h"
#include "ofp/writable.h"
#include "ofp/message.h"
#include "ofp/mpflowstatsrequest.h"
#include "ofp/mpaggregatestatsrequest.h"
#include "ofp/mpportstatsrequest.h"
#include "ofp/mpqueuestatsrequest.h"
#include "ofp/mpgroupstatsrequest.h"
#include "ofp/mpmeterstatsrequest.h"
#include "ofp/mptablefeatures.h"
#include "ofp/mpexperimenter.h"

using namespace ofp;


bool MultipartRequest::validateInput(Validation *context) const {
  context->setLengthRemaining(requestBodySize());

  switch (requestType()) {
    case OFPMP_DESC:
      return context->validateEmpty(requestBody(), OFP_VERSION_1);
    case OFPMP_FLOW:
      return context->validate<MPFlowStatsRequest>(requestBody(), OFP_VERSION_1);
    case OFPMP_AGGREGATE:
      return context->validate<MPAggregateStatsRequest>(requestBody(), OFP_VERSION_1);
    case OFPMP_TABLE:
      return context->validateEmpty(requestBody(), OFP_VERSION_1);
    case OFPMP_PORT_STATS:
      return context->validate<MPPortStatsRequest>(requestBody(), OFP_VERSION_1);
    case OFPMP_QUEUE:
      return context->validate<MPQueueStatsRequest>(requestBody(), OFP_VERSION_1);
    case OFPMP_GROUP:
      return context->validate<MPGroupStatsRequest>(requestBody(), OFP_VERSION_2);
    case OFPMP_GROUP_DESC:
      return context->validateEmpty(requestBody(), OFP_VERSION_2);
    case OFPMP_GROUP_FEATURES:
      return context->validateEmpty(requestBody(), OFP_VERSION_3);
    case OFPMP_METER:
      return context->validate<MPMeterStatsRequest>(requestBody(), OFP_VERSION_4);
    case OFPMP_METER_CONFIG:
      return context->validate<MPMeterConfigRequest>(requestBody(), OFP_VERSION_4);
    case OFPMP_METER_FEATURES:
      return context->validateEmpty(requestBody(), OFP_VERSION_4);
    case OFPMP_TABLE_FEATURES:
      return context->validateArrayVariableSize<MPTableFeatures>(requestBody(), OFP_VERSION_4);
    case OFPMP_PORT_DESC:
      return context->validateEmpty(requestBody(), OFP_VERSION_4);
    case OFPMP_EXPERIMENTER:
      return context->validate<MPExperimenter>(requestBody(), OFP_VERSION_1, false);
  }

  context->multipartTypeIsNotSupported();

  return false;
}

UInt32 MultipartRequestBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();
  
  size_t msgLen = body_.size();
  if (version == OFP_VERSION_1) {
    msgLen += MultipartRequest::UnpaddedSizeVersion1;
  } else {
    msgLen += sizeof(msg_);
  }

  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  if (version == OFP_VERSION_1) {
    msg_.header_.setType(deprecated::v1::OFPT_STATS_REQUEST);
    channel->write(&msg_, MultipartRequest::UnpaddedSizeVersion1);
  } else {
    msg_.header_.setType(OFPT_MULTIPART_REQUEST);
    channel->write(&msg_, sizeof(msg_));
  }
  
  channel->write(body_.data(), body_.size());
  channel->flush();

  return xid;
}

