// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/packetoutv6.h"

#include "ofp/message.h"

using namespace ofp;

bool PacketOutV6::validateInput(Validation *context) const {
  size_t length = context->length();

  if (length < UnpaddedSizeWithMatchHeader) {
    log_debug("PacketOutV6 too small", length);
    return false;
  }

  size_t remainingLength = length - SizeWithoutMatchHeader;
  if (!matchHeader_.validateInput(remainingLength, context)) {
    log_info("PacketOutV6: invalid match");
    return false;
  }

  // Check actionsLen_.
  assert(remainingLength >= matchHeader_.paddedLength());
  remainingLength -= matchHeader_.paddedLength();

  if (remainingLength < actionsLen_) {
    log_info("PacketOutv6: invalid actionsLen");
    return false;
  }
  if (!actions().validateInput(context)) {
    log_info("PacketOutV6: invalid actions");
    return false;
  }

  return true;
}

ActionRange PacketOutV6::actions() const {
  size_t offset = SizeWithoutMatchHeader + matchHeader_.paddedLength();
  return SafeByteRange(this, header_.length(), offset, actionsLen_);
}

ByteRange PacketOutV6::enetFrame() const {
  size_t offset =
      SizeWithoutMatchHeader + matchHeader_.paddedLength() + actionsLen_;
  return SafeByteRange(this, header_.length(), offset);
}

PacketOutV6Builder::PacketOutV6Builder(const PacketOutV6 *msg) : msg_{*msg} {
  setMatch(msg->match());
  setActions(msg->actions());
  setEnetFrame(msg->enetFrame());
}

UInt32 PacketOutV6Builder::send(Writable *channel) {
  UInt8 version = channel->version();

  // Calculate length of PacketOut message up to end of match section. Then
  // pad it to a multiple of 8 bytes.
  size_t msgMatchLen = PacketOutV6::UnpaddedSizeWithMatchHeader + match_.size();
  size_t msgMatchLenPadded = PadLength(msgMatchLen);

  size_t actionSize = actions_.toRange().writeSize(channel);
  size_t msgLen = msgMatchLenPadded + actionSize + enetFrame_.size();

  // Fill in the message header.
  UInt32 xid = channel->nextXid();
  Header &hdr = msg_.header_;
  hdr.setVersion(version);
  hdr.setType(PacketOutV6::type());
  hdr.setLength(msgLen);
  hdr.setXid(xid);

  // Fill in length of actions section.
  msg_.actionsLen_ = UInt16_narrow_cast(actionSize);

  // Fill in the match header.
  msg_.matchHeader_.setType(OFPMT_OXM);
  msg_.matchHeader_.setLength(sizeof(MatchHeader) + match_.size());

  // Write the message with padding in the correct spots.
  channel->write(&msg_, PacketOutV6::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size(), msgMatchLenPadded - msgMatchLen);

  // Write actions to channel.
  actions_.toRange().write(channel);

  channel->write(enetFrame_.data(), enetFrame_.size());
  channel->flush();

  return xid;
}
