// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpflowstatsreply.h"

#include "ofp/originalmatch.h"
#include "ofp/validation.h"
#include "ofp/writable.h"

using namespace ofp;

Match MPFlowStatsReply::match() const {
  return Match{&matchHeader_};
}

InstructionRange MPFlowStatsReply::instructions() const {
  assert(matchHeader_.type() == OFPMT_OXM ||
         matchHeader_.type() == OFPMT_STANDARD);

  size_t offset = SizeWithoutMatchHeader + matchHeader_.paddedLength();
  assert(length_ >= offset);

  return InstructionRange{SafeByteRange(this, length_, offset)};
}

bool MPFlowStatsReply::validateInput(Validation *context) const {
  size_t length = length_;
  if (!context->validateAlignedLength(length, UnpaddedSizeWithMatchHeader)) {
    return false;
  }

  if (!matchHeader_.validateInput(length - SizeWithoutMatchHeader, context)) {
    return false;
  }

  context->setLengthRemaining(length - SizeWithoutMatchHeader -
                              matchHeader_.length());

  return instructions().validateInput(context);
}

void MPFlowStatsReplyBuilder::write(Writable *channel) {
  UInt8 version = channel->version();

  if (version == OFP_VERSION_1) {
    writeV1(channel);
    return;
  }

  size_t msgMatchLen =
      MPFlowStatsReply::UnpaddedSizeWithMatchHeader + match_.size();
  size_t msgMatchLenPadded = PadLength(msgMatchLen);

  size_t msgLen = msgMatchLenPadded + instructions_.size();

  msg_.length_ = UInt16_narrow_cast(msgLen);
  msg_.matchHeader_.setType(OFPMT_OXM);
  msg_.matchHeader_.setLength(sizeof(MatchHeader) + match_.size());

  channel->write(&msg_, MPFlowStatsReply::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size(), msgMatchLenPadded - msgMatchLen);
  channel->write(instructions_.data(), instructions_.size());
  channel->flush();
}

void MPFlowStatsReplyBuilder::writeV1(Writable *channel) {
  deprecated::OriginalMatch origMatch{match_.toRange()};
  ActionRange actions = instructions_.toRange().outputActions();

  size_t msgLen = 88 + actions.writeSize(channel);

  msg_.length_ = UInt16_narrow_cast(msgLen);
  msg_.flags_ = OFPFF_NONE;

  channel->write(&msg_, 4);
  channel->write(&origMatch, sizeof(origMatch));
  channel->write(&msg_.duration_, 44);
  if (actions.size() > 0) {
    actions.write(channel);
  }
  channel->flush();
}

void MPFlowStatsReplyBuilder::reset() {
  match_.clear();
  instructions_.clear();
}
