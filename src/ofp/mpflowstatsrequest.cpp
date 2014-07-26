//  ===== ---- ofp/flowstatsrequest.cpp --------------------*- C++ -*- =====  //
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
/// \brief Implements MPFlowStatsRequest and MPFlowStatsRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/mpflowstatsrequest.h"
#include "ofp/writable.h"
#include "ofp/multipartrequest.h"
#include "ofp/originalmatch.h"

using namespace ofp;

const MPFlowStatsRequest *MPFlowStatsRequest::cast(const MultipartRequest *req) {
  return req->body_cast<MPFlowStatsRequest>();
}

bool MPFlowStatsRequest::validateInput(Validation *context) const {
  size_t length = context->lengthRemaining();
  
  if (length < SizeWithoutMatchHeader) {
    log::debug("MPFlowStatsRequest: Too short.");
    return false;
  }

  if (!matchHeader_.validateInput(length - SizeWithoutMatchHeader)) {
    log::debug("MPFlowStatsRequest: Invalid match.");
    return false;
  }

  if (length != SizeWithoutMatchHeader + matchHeader_.paddedLength()) {
    log::debug("MPFlowStatsRequest: Invalid length.");
    return false;
  }

  return true;
}

Match MPFlowStatsRequest::match() const {
  return Match{&matchHeader_};
}

void MPFlowStatsRequestBuilder::write(Writable *channel) {
  UInt8 version = channel->version();

  if (version == OFP_VERSION_1) {
    writeV1(channel);
    return;
  }

  size_t matchLen = sizeof(MatchHeader) + match_.size();
  size_t matchLenPadded = PadLength(matchLen);

  msg_.matchHeader_.setType(OFPMT_OXM);
  msg_.matchHeader_.setLength(matchLen);

  channel->write(&msg_, MPFlowStatsRequest::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size(), matchLenPadded - matchLen);
  channel->flush();
}

void MPFlowStatsRequestBuilder::writeV1(Writable *channel) {
  deprecated::OriginalMatch origMatch{match_.toRange()};

  channel->write(&origMatch, sizeof(origMatch));

  struct Block {
    Big8 tableId;
    Padding<1> pad;
    Big16 outPort;
  };
  static_assert(sizeof(Block) == 4, "Unexpected size.");

  Block info;
  info.tableId = msg_.tableId_;
  info.outPort = UInt16_narrow_cast(msg_.outPort_);
  channel->write(&info, sizeof(info));

  channel->flush();
}
