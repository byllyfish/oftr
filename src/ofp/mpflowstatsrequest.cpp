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

bool MPFlowStatsRequest::validateLength(size_t length) const {
  if (length < sizeof(MPFlowStatsRequest)) {
    log::debug("MPFlowStatsRequest: Validation failed.");
    return false;
  }

  return true;
}

Match MPFlowStatsRequest::match() const {
  UInt16 type = matchType_;

  if (type == OFPMT_OXM) {
    OXMRange range{BytePtr(this) + UnpaddedSizeWithMatchHeader,
                   matchLength_ - HeaderSize};
    return Match{range};

  } else if (type == OFPMT_STANDARD) {
    const deprecated::StandardMatch *stdMatch =
        reinterpret_cast<const deprecated::StandardMatch *>(
            BytePtr(this) + SizeWithoutMatchHeader);
    return Match{stdMatch};

  } else {
    log::debug("MPFlowStatsRequest: Unknown matchType:", type);
    return Match{OXMRange{nullptr, 0}};
  }
}

void MPFlowStatsRequestBuilder::write(Writable *channel) {
  UInt8 version = channel->version();

  if (version == OFP_VERSION_1) {
    writeV1(channel);
    return;
  }

  size_t matchLen = MPFlowStatsRequest::HeaderSize + match_.size();
  size_t matchLenPadded = PadLength(matchLen);

  msg_.matchType_ = OFPMT_OXM;
  msg_.matchLength_ = UInt16_narrow_cast(matchLen);

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
