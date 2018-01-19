// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpaggregatestatsreplyv6.h"
#include "ofp/oxmfields.h"
#include "ofp/validation.h"
#include "ofp/writable.h"

using namespace ofp;

UInt64 MPAggregateStatsReplyV6::packetCount() const {
  return statHeader()->oxmRange().get<OXS_PACKET_COUNT>();
}

UInt64 MPAggregateStatsReplyV6::byteCount() const {
  return statHeader()->oxmRange().get<OXS_BYTE_COUNT>();
}

UInt32 MPAggregateStatsReplyV6::flowCount() const {
  return statHeader()->oxmRange().get<OXS_FLOW_COUNT>();
}

Stat MPAggregateStatsReplyV6::stat() const {
  return Stat{statHeader()};
}

bool MPAggregateStatsReplyV6::validateInput(Validation *context) const {
  size_t length = context->length();

  if (length < sizeof(StatHeader)) {
    return false;
  }

  const StatHeader *stat = statHeader();
  if (!stat->validateInput(length, context)) {
    log_warning("MPAggregateStatsReply: invalid stat");
    return false;
  }

  return true;
}

const StatHeader *MPAggregateStatsReplyV6::statHeader() const {
  return Interpret_cast<StatHeader>(BytePtr(&stats_));
}

void MPAggregateStatsReplyV6Builder::setPacketCount(UInt64 packetCount) {
  stats_.add(OXS_PACKET_COUNT{packetCount});
}

void MPAggregateStatsReplyV6Builder::setByteCount(UInt64 byteCount) {
  stats_.add(OXS_BYTE_COUNT{byteCount});
}

void MPAggregateStatsReplyV6Builder::setFlowCount(UInt32 flowCount) {
  stats_.add(OXS_FLOW_COUNT{flowCount});
}

void MPAggregateStatsReplyV6Builder::write(Writable *channel) {
  const size_t len = sizeof(StatHeader) + stats_.size();
  const size_t paddedLen = PadLength(len);

  StatHeader statHeader;
  statHeader.setLength(len);

  channel->write(&statHeader, sizeof(statHeader));
  channel->write(stats_.data(), stats_.size(), paddedLen - len);
  channel->flush();
}
