//  ===== ---- ofp/mpflowstatsreply.cpp --------------------*- C++ -*- =====  //
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
/// \brief Implements MPFlowStatsReply and MPFlowStatsReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/mpflowstatsreply.h"
#include "ofp/writable.h"
#include "ofp/originalmatch.h"

using namespace ofp;

Match MPFlowStatsReply::match() const {
  UInt16 type = matchType_;

  if (type == OFPMT_OXM) {
    OXMRange range{BytePtr(this) + UnpaddedSizeWithMatchHeader, matchLength_ - MatchHeaderSize};
    return Match{range};

  } else if (type == OFPMT_STANDARD) {
    const deprecated::StandardMatch *stdMatch =
        reinterpret_cast<const deprecated::StandardMatch *>(
            BytePtr(this) + SizeWithoutMatchHeader);
    return Match{stdMatch};

  } else {
    log::debug("MPFlowStatsReply: Unknown matchType:", type);
    return Match{OXMRange{nullptr, 0}};
  }
}

InstructionRange MPFlowStatsReply::instructions() const {
  assert(matchType_ == OFPMT_OXM || matchType_ == OFPMT_STANDARD);

  size_t offset = PadLength(SizeWithoutMatchHeader + matchLength_);
  assert(length_ >= offset);

  return InstructionRange{ByteRange{BytePtr(this) + offset, length_ - offset}};
}

void MPFlowStatsReplyBuilder::write(Writable *channel) {
  UInt8 version = channel->version();

  if (version == OFP_VERSION_1) {
    writeV1(channel);
    return;
  }

  size_t msgMatchLen = MPFlowStatsReply::UnpaddedSizeWithMatchHeader + match_.size();
  size_t msgMatchLenPadded = PadLength(msgMatchLen);

  size_t msgLen = msgMatchLenPadded + instructions_.size();

  msg_.length_ = UInt16_narrow_cast(msgLen);
  msg_.matchType_ = OFPMT_OXM;
  msg_.matchLength_ = UInt16_narrow_cast(MPFlowStatsReply::MatchHeaderSize + match_.size());

  channel->write(&msg_, MPFlowStatsReply::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size(), msgMatchLenPadded - msgMatchLen);
  channel->write(instructions_.data(), instructions_.size());
  channel->flush();
}


void MPFlowStatsReplyBuilder::writeV1(Writable *channel)
{
  deprecated::OriginalMatch origMatch{match_.toRange()};
  ActionRange actions = instructions_.toRange().outputActions();

  size_t msgLen = 88 + actions.writeSize(channel);

  msg_.length_ = UInt16_narrow_cast(msgLen);
  msg_.flags_ = 0;

  channel->write(&msg_, 4);
  channel->write(&origMatch, sizeof(origMatch));
  channel->write(&msg_.durationSec_, 44);
  if (actions.size() > 0) {
    actions.write(channel);
  }
  channel->flush();
}
