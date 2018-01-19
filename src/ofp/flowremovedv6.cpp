// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/flowremovedv6.h"
#include "ofp/statheader.h"
#include "ofp/writable.h"

using namespace ofp;

DurationSec FlowRemovedV6::duration() const {
  return statHeader()->oxmRange().get<OXS_DURATION>();
}

UInt64 FlowRemovedV6::packetCount() const {
  return statHeader()->oxmRange().get<OXS_PACKET_COUNT>();
}

UInt64 FlowRemovedV6::byteCount() const {
  return statHeader()->oxmRange().get<OXS_BYTE_COUNT>();
}

Match FlowRemovedV6::match() const {
  return Match{&matchHeader_};
}

Stat FlowRemovedV6::stat() const {
  return Stat{statHeader()};
}

const StatHeader *FlowRemovedV6::statHeader() const {
  return Interpret_cast<StatHeader>(BytePtr(&matchHeader_) +
                                    matchHeader_.paddedLength());
}

bool FlowRemovedV6::validateInput(Validation *context) const {
  size_t length = context->length();

  if (length < UnpaddedSizeWithMatchHeader) {
    log_debug("FlowRemovedV6 too small", length);
    return false;
  }

  size_t remainingLength = length - SizeWithoutMatchHeader;
  if (!matchHeader_.validateInput(remainingLength, context)) {
    log_info("FlowRemovedV6: invalid match");
    return false;
  }

  size_t paddedMatchLen = matchHeader_.paddedLength();
  if (remainingLength < paddedMatchLen) {
    log_error("FlowRemovedV6: invalid remaining length");
    return false;
  }

  remainingLength -= paddedMatchLen;

  const StatHeader *stat =
      Interpret_cast<StatHeader>(BytePtr(&matchHeader_) + paddedMatchLen);
  if (!stat->validateInput(remainingLength, context)) {
    log_info("FlowRemovedV6: invalid stat");
    return false;
  }

  if (remainingLength != stat->paddedLength()) {
    log_info("FlowRemovedV6: extra data");
    return false;
  }

  return true;
}

void FlowRemovedV6Builder::setPacketCount(UInt64 packetCount) {
  stat_.add(OXS_PACKET_COUNT{packetCount});
}

void FlowRemovedV6Builder::setByteCount(UInt64 byteCount) {
  stat_.add(OXS_BYTE_COUNT{byteCount});
}

void FlowRemovedV6Builder::setDuration(const DurationSec &duration) {
  stat_.add(OXS_DURATION{duration});
}

UInt32 FlowRemovedV6Builder::send(Writable *channel) {
  // Calculate length of FlowRemoved message up to end of match section. Then
  // pad it to a multiple of 8 bytes.
  size_t msgMatchLen =
      FlowRemovedV6::UnpaddedSizeWithMatchHeader + match_.size();
  size_t msgMatchLenPadded = PadLength(msgMatchLen);

  size_t msgStatLen = msgMatchLenPadded + sizeof(StatHeader) + stat_.size();
  size_t msgStatLenPadded = PadLength(msgStatLen);

  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();
  size_t msgLen = msgStatLenPadded;

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);
  msg_.header_.setXid(xid);

  // Fill in the match header.
  msg_.matchHeader_.setType(OFPMT_OXM);
  msg_.matchHeader_.setLength(sizeof(MatchHeader) + match_.size());

  channel->write(&msg_, FlowRemovedV6::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size(), msgMatchLenPadded - msgMatchLen);

  StatHeader statHeader;
  statHeader.setLength(sizeof(StatHeader) + stat_.size());

  channel->write(&statHeader, sizeof(statHeader));
  channel->write(stat_.data(), stat_.size(), msgStatLenPadded - msgStatLen);

  channel->flush();

  return xid;
}
