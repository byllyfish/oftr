// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/packetin.h"
#include "ofp/writable.h"

using namespace ofp;

// Offset Table for backward compatibility (offset, name, size)
//
// Version 1:               Version 2:                  Version 3: (*)
//    0  header      8 ^       0  header      8 ^          0  header     8 ^
//    8  buffer_id   4 ^       8  buffer_id   4 ^          8  buffer_id  4 ^
//   12  total_len   2 ^      12  in_port     4           12  total_len  2 ^
//   14  in_port     2        16  in_phy_port 4           14  reason     1 ^
//   16  reason      1        20  total_len   2           15  table_id   1 ^
//   17  pad         1        22  reason      1           16  match_type 2
//   18  enet_frame  x        23  table_id    1           18  match_len  2
//                            24  pad         2 (?)       20  oxm...     x
//                            26  enet_frame  x
//
// (^) Field is same offset and size as version 4.
//
// (?) From the spec, it sounds like the enetframe is supposed to start between
// a four-byte boundary (64 bit + 16)?
//
// (*) Version 3 is same as version 4, but without the 64 bit cookie field
// between table_id and match_type.

bool PacketIn::validateInput(Validation *context) const {
  switch (version()) {
    case OFP_VERSION_1:
      return validateInputV1(context);
    case OFP_VERSION_2:
      return validateInputV2(context);
    case OFP_VERSION_3:
      return validateInputV3(context);
    default:
      break;
  }

  size_t length = context->length();

  if (length < SizeWithoutMatchHeader) {
    context->messageSizeIsInvalid();
    return false;
  }

  if (!matchHeader_.validateInput(length - SizeWithoutMatchHeader)) {
    // FIXME(bfish)
    log::info("PacketIn has invalid Match element.");
    return false;
  }

  return true;
}

bool PacketIn::validateInputV1(Validation *context) const {
  size_t length = context->length();
  if (length < 18) {
    context->messageSizeIsInvalid();
    return false;
  }

  return true;
}

bool PacketIn::validateInputV2(Validation *context) const {
  // FIXME - Unimplemented
  context->messageTypeIsNotSupported();
  return false;
}

bool PacketIn::validateInputV3(Validation *context) const {
  size_t length = context->length();
  if (length < 24) {
    context->messageSizeIsInvalid();
    return false;
  }

  const MatchHeader *matchHdr = matchHeader();

  if (!matchHdr->validateInput(length - 16)) {
    log::info("PacketIn has invalid Match element.");
    return false;
  }

  return true;
}

UInt16 PacketIn::totalLen() const {
  switch (version()) {
    case OFP_VERSION_2:
      return offset<Big16>(20);
    default:
      return totalLen_;
  }
}

OFPPacketInReason PacketIn::reason() const {
  switch (version()) {
    case OFP_VERSION_1:
      return offset<OFPPacketInReason>(16);
    case OFP_VERSION_2:
      return offset<OFPPacketInReason>(22);
    default:
      return reason_;
  }
}

UInt8 PacketIn::tableID() const {
  switch (version()) {
    case OFP_VERSION_1:
      return 0;
    case OFP_VERSION_2:
      return offset<Big8>(23);
    default:
      return tableID_;
  }
}

UInt32 PacketIn::inPort() const {
  switch (version()) {
    case OFP_VERSION_1:
      return offset<Big16>(14);
    case OFP_VERSION_2:
      return offset<Big32>(12);
    default:
      return matchHeader()->oxmRange().get<OFB_IN_PORT>();
  }
}

UInt32 PacketIn::inPhyPort() const {
  switch (version()) {
    case OFP_VERSION_1:
      return 0;
    case OFP_VERSION_2:
      return offset<Big32>(16);
    default: {
      // If not present or zero, assume the value is identical to OFB_IN_PORT.
      OXMRange oxm = matchHeader()->oxmRange();
      UInt32 inPhyPort = oxm.get<OFB_IN_PHY_PORT>();
      if (inPhyPort) 
        return inPhyPort;
      return oxm.get<OFB_IN_PORT>();
    }
  }
}

UInt64 PacketIn::metadata() const {
  switch (version()) {
    case OFP_VERSION_1:
    case OFP_VERSION_2:
      return 0;
    default:
      return matchHeader()->oxmRange().get<OFB_METADATA>();
  }
}

UInt64 PacketIn::cookie() const {
  switch (version()) {
    case OFP_VERSION_1:
    case OFP_VERSION_2:
    case OFP_VERSION_3:
      return 0;
    default:
      return cookie_;
  }
}

Match PacketIn::match() const {
  switch (version()) {
    case OFP_VERSION_1:
    case OFP_VERSION_2:
      return Match{};
    default:
      return Match{matchHeader()};
  }
}

ByteRange PacketIn::enetFrame() const {
  size_t offset;
  size_t msgLen = header_.length();

  switch (version()) {
    case OFP_VERSION_1:
      assert(msgLen >= 18U);
      return ByteRange{BytePtr(this) + 18, msgLen - 18U};
    case OFP_VERSION_2:
      log::info("PacketIn::enetFrame() not implemented.");
      return ByteRange{};
    case OFP_VERSION_3: {
      const MatchHeader *matchHdr = matchHeader();
      offset =
          SizeWithoutMatchHeader - sizeof(Big64) + matchHdr->paddedLength() + 2;
      if (offset >= msgLen) {
        return ByteRange{};
      }
      return ByteRange{BytePtr(this) + offset, msgLen - offset};
    }
    default:
      offset = SizeWithoutMatchHeader + matchHeader_.paddedLength() + 2;
      if (offset >= msgLen) {
        return ByteRange{};
      }
      return ByteRange{BytePtr(this) + offset, msgLen - offset};
  }
}

const MatchHeader *PacketIn::matchHeader() const {
  assert(version() >= OFP_VERSION_3);

  switch (version()) {
    case OFP_VERSION_3:
      return reinterpret_cast<const MatchHeader *>(BytePtr(this) + 16);
    case OFP_VERSION_4:
    default:
      return &matchHeader_;
  }
}

PacketInBuilder::PacketInBuilder(const PacketIn *msg) {
  // Use accessor functions only; PacketIn uses cross-wired accessors.

  setBufferId(msg->bufferId());
  setTotalLen(msg->totalLen());
  setInPort(msg->inPort());
  setInPhyPort(msg->inPhyPort());
  setMetadata(msg->metadata());
  setReason(msg->reason());
  setTableID(msg->tableID());
  setCookie(msg->cookie());
  setEnetFrame(msg->enetFrame());
}

UInt32 PacketInBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  switch (version) {
    case OFP_VERSION_1:
      return sendV1(channel);
    case OFP_VERSION_2:
      return sendV2(channel);
    case OFP_VERSION_3:
      return sendV3(channel);
    default:
      return sendV4(channel);
  }
}

UInt32 PacketInBuilder::sendV1(Writable *channel) {
  UInt8 version = channel->version();
  assert(version == OFP_VERSION_1);

  // Calculate the total PacketIn message length.
  size_t msgLen = 18 + enetFrame_.size();

  // Fill in the header.
  UInt32 xid = channel->nextXid();
  Header &hdr = msg_.header_;
  hdr.setVersion(version);
  hdr.setType(PacketIn::type());
  hdr.setLength(UInt16_narrow_cast(msgLen));
  hdr.setXid(xid);

  // Write out first 14 bytes.
  channel->write(&msg_, 14);

  // Write out 16-bit port number.
  channel->write(BytePtr(&inPort_) + 2, 2);

  // Write out 8-bit reason and 1 byte pad.
  channel->write(&msg_.reason_, 1);
  channel->write(&msg_.pad_, 1);

  // Write out enet frame.
  channel->write(enetFrame_.data(), enetFrame_.size());
  channel->flush();

  return xid;
}

UInt32 PacketInBuilder::sendV2(Writable *channel) {
  assert(channel->version() == OFP_VERSION_2);

  UInt32 xid = channel->nextXid();

  // FIXME - Unimplemented
  log::info("PacketInBuilder::sendV2 not implemented.");

  return xid;
}

UInt32 PacketInBuilder::sendV3(Writable *channel) {
  assert(channel->version() == OFP_VERSION_3);

  // Construct the match with the standard context fields: IN_PORT, IN_PHY_PORT,
  // METADATA, and TUNNEL_ID.
  //
  // Fields whose values are all-bits-zero should be omitted. IN_PHY_PORT should
  // be omitted if it has the same value as IN_PORT.

  // TODO(bfish): check ordering of fields in resulting match. Especially if
  // there are other fields already added.

  if (inPort_) match_.replaceUnchecked(OFB_IN_PORT{inPort_});

  if (inPhyPort_ && (inPhyPort_ != inPort_))
    match_.replaceUnchecked(OFB_IN_PHY_PORT{inPhyPort_});

  if (metadata_) match_.replaceUnchecked(OFB_METADATA{metadata_});

  // TODO(bfish): TUNNEL_ID

  // Calculate length of PacketIn message up to end of match section. Then
  // pad it to a multiple of 8 bytes.
  size_t msgMatchLen =
      PacketIn::UnpaddedSizeWithMatchHeader - sizeof(Big64) + match_.size();
  size_t msgMatchLenPadded = PadLength(msgMatchLen);

  // Calculate length of ethernet frame section (preceded by 2 byte pad.)
  size_t enetFrameLen = enetFrame_.size() + 2;

  // Calculate the total PacketIn message length.
  size_t msgLen = msgMatchLenPadded + enetFrameLen;

  // Fill in the message header.
  UInt32 xid = channel->nextXid();
  Header &hdr = msg_.header_;
  hdr.setVersion(OFP_VERSION_3);
  hdr.setType(PacketIn::type());
  hdr.setLength(UInt16_narrow_cast(msgLen));
  hdr.setXid(xid);

  // Fill in the match header.
  msg_.matchHeader_.setType(OFPMT_OXM);
  msg_.matchHeader_.setLength(sizeof(MatchHeader) + match_.size());

  // Write the message with padding in the correct spots.
  Padding<8> pad;
  channel->write(&msg_, PacketIn::SizeWithoutMatchHeader - sizeof(Big64));
  channel->write(&msg_.matchHeader_, sizeof(MatchHeader));
  channel->write(match_.data(), match_.size());
  channel->write(&pad, msgMatchLenPadded - msgMatchLen);
  channel->write(&pad, 2);
  channel->write(enetFrame_.data(), enetFrame_.size());
  channel->flush();

  return xid;
}

UInt32 PacketInBuilder::sendV4(Writable *channel) {
  assert(channel->version() >= OFP_VERSION_4);

  // Construct the match with the standard context fields: IN_PORT, IN_PHY_PORT,
  // METADATA, and TUNNEL_ID.
  //
  // Fields whose values are all-bits-zero should be omitted. IN_PHY_PORT should
  // be omitted if it has the same value as IN_PORT.

  // TODO(bfish): check ordering of fields in resulting match. Especially if
  // there are other fields already added.

  if (inPort_) match_.replaceUnchecked(OFB_IN_PORT{inPort_});

  if (inPhyPort_ && (inPhyPort_ != inPort_))
    match_.replaceUnchecked(OFB_IN_PHY_PORT{inPhyPort_});

  if (metadata_) match_.replaceUnchecked(OFB_METADATA{metadata_});

  // TODO(bfish): TUNNEL_ID

  // Calculate length of PacketIn message up to end of match section. Then
  // pad it to a multiple of 8 bytes.
  size_t msgMatchLen = PacketIn::UnpaddedSizeWithMatchHeader + match_.size();
  size_t msgMatchLenPadded = PadLength(msgMatchLen);

  // Calculate length of ethernet frame section (preceded by 2 byte pad.)
  size_t enetFrameLen = enetFrame_.size() + 2;

  // Calculate the total PacketIn message length.
  size_t msgLen = msgMatchLenPadded + enetFrameLen;

  // Fill in the message header.
  UInt32 xid = channel->nextXid();
  Header &hdr = msg_.header_;
  hdr.setVersion(channel->version());
  hdr.setType(PacketIn::type());
  hdr.setLength(UInt16_narrow_cast(msgLen));
  hdr.setXid(xid);

  // Fill in the match header.
  msg_.matchHeader_.setType(OFPMT_OXM);
  msg_.matchHeader_.setLength(sizeof(MatchHeader) + match_.size());

  // Write the message with padding in the correct spots.
  Padding<8> pad;
  channel->write(&msg_, PacketIn::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size());
  channel->write(&pad, msgMatchLenPadded - msgMatchLen);
  channel->write(&pad, 2);
  channel->write(enetFrame_.data(), enetFrame_.size());
  channel->flush();

  return xid;
}
