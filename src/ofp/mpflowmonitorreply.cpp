#include "ofp/mpflowmonitorreply.h"
#include "ofp/validation.h"

using namespace ofp;

Match MPFlowMonitorReply::match() const {
  assert(full());
  return Match{&matchHeader_};
}

InstructionRange MPFlowMonitorReply::instructions() const {
  assert(full());

  assert(matchHeader_.type() == OFPMT_OXM ||
         matchHeader_.type() == OFPMT_STANDARD);

  size_t offset = SizeWithoutMatchHeader + matchHeader_.paddedLength();
  assert(length_ >= offset);

  return InstructionRange{ByteRange{BytePtr(this) + offset, length_ - offset}};
}

bool MPFlowMonitorReply::validateInput(Validation *context) const {
  size_t lengthRemaining = context->lengthRemaining();

  if (lengthRemaining < OFPFME_ABBREV) {
    return false;
  }

  if (event_ >= OFPFME_ABBREV) {
    return (length_ == AbbrevSize);
  }

  size_t length = length_;
  if (length_ > lengthRemaining) {
    return false;
  }

  if (length < SizeWithoutMatchHeader) {
    log::debug("MPFlowMonitorReply: Too short.");
    return false;
  }

  if (!matchHeader_.validateInput(length - SizeWithoutMatchHeader)) {
    log::debug("MPFlowMonitorReply: Invalid match.");
    return false;
  }

  if (length < SizeWithoutMatchHeader + matchHeader_.paddedLength()) {
    log::debug("MPFlowMonitorReply: Invalid length.");
    return false;
  }

  return true;
}

void MPFlowMonitorReplyBuilder::write(Writable *channel) {
  if (msg_.event_ >= OFPFME_ABBREV) {
    writeAbbrev(channel);
    return;
  }

  size_t msgMatchLen =
      MPFlowMonitorReply::UnpaddedSizeWithMatchHeader + match_.size();
  size_t msgMatchLenPadded = PadLength(msgMatchLen);
  size_t msgLen = msgMatchLenPadded + instructions_.size();

  msg_.length_ = UInt16_narrow_cast(msgLen);
  msg_.matchHeader_.setType(OFPMT_OXM);
  msg_.matchHeader_.setLength(sizeof(MatchHeader) + match_.size());

  channel->write(&msg_, MPFlowMonitorReply::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size(), msgMatchLenPadded - msgMatchLen);
  channel->write(instructions_.data(), instructions_.size());
  channel->flush();
}

void MPFlowMonitorReplyBuilder::reset() {
  match_.clear();
  instructions_.clear();
}

void MPFlowMonitorReplyBuilder::writeAbbrev(Writable *channel) {
  assert(msg_.event_ >= OFPFME_ABBREV);

  if (msg_.event_ != OFPFME_ABBREV) {
    setXid(0);
  }

  msg_.length_ = MPFlowMonitorReply::AbbrevSize;

  channel->write(&msg_, MPFlowMonitorReply::AbbrevSize);
  channel->flush();
}
