//  ===== ---- ofp/packetin.cpp ----------------------------*- C++ -*- =====  //
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
/// \brief Implements PacketIn and PacketInBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

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



bool PacketIn::validateLength(size_t length) const
{
    switch (version()) {
    case OFP_VERSION_1:
        return validateLengthV1(length);
    case OFP_VERSION_2:
        return validateLengthV2(length);
    case OFP_VERSION_3:
        return validateLengthV3(length);
    default:
        break;
    }

    if (length < UnpaddedSizeWithMatchHeader) {
        log::debug("PacketIn too small.");
        return false;
    }

    // Check the match length.
    UInt16 matchLen = matchLength_;
    if (length < UnpaddedSizeWithMatchHeader + matchLen) {
        log::debug("PacketIn has mismatched lengths.");
        return false;
    }

    return true;
}

bool PacketIn::validateLengthV1(size_t length) const
{
    if (length < 18) {
        return false;
    }

    return true;
}

bool PacketIn::validateLengthV2(size_t length) const
{
    // FIXME - Unimplemented
    return false;
}

bool PacketIn::validateLengthV3(size_t length) const
{
    // FIXME - Unimplemented
    return false;
}

UInt16 PacketIn::totalLen() const
{
    switch (version()) {
    case OFP_VERSION_2:
        return offset<Big16>(20);
    default:
        return totalLen_;
    }
}

OFPPacketInReason PacketIn::reason() const
{
    switch (version()) {
    case OFP_VERSION_1:
        return offset<OFPPacketInReason>(16);
    case OFP_VERSION_2:
        return offset<OFPPacketInReason>(22);
    default:
        return reason_;
    }
}

UInt8 PacketIn::tableID() const
{
    switch (version()) {
    case OFP_VERSION_1:
        return 0;
    case OFP_VERSION_2:
        return offset<Big8>(23);
    default:
        return tableID_;
    }
}

UInt32 PacketIn::inPort() const
{
    switch (version()) {
    case OFP_VERSION_1:
        return offset<Big16>(14);
    case OFP_VERSION_2:
        return offset<Big32>(12);
    default:
        return oxmRange().get<OFB_IN_PORT>();
    }
}

UInt32 PacketIn::inPhyPort() const
{
    switch (version()) {
    case OFP_VERSION_1:
        return 0;
    case OFP_VERSION_2:
        return offset<Big32>(16);
    default:
        return oxmRange().get<OFB_IN_PHY_PORT>();
    }
}

UInt64 PacketIn::metadata() const
{
    switch (version()) {
    case OFP_VERSION_1:
    case OFP_VERSION_2:
        return 0;
    default:
        return oxmRange().get<OFB_METADATA>();
    }
}

UInt64 PacketIn::cookie() const
{
    switch (version()) {
    case OFP_VERSION_1:
    case OFP_VERSION_2:
    case OFP_VERSION_3:
        return 0;
    default:
        return cookie_;
    }
}


ByteRange PacketIn::enetFrame() const
{
    size_t offset;

    switch (version()) {
    case OFP_VERSION_1:
        return ByteRange{BytePtr(this) + 18, header_.length() - 18U};
    case OFP_VERSION_4:
        offset = PadLength(SizeWithoutMatchHeader + matchLength_) + 2;
        return ByteRange{BytePtr(this) + offset, header_.length() - offset};
    default:
        return ByteRange{};
    }
}

OXMRange PacketIn::oxmRange() const
{
    assert(version() >= OFP_VERSION_3);

    switch (version()) {
    case OFP_VERSION_1:
    case OFP_VERSION_2:
        return OXMRange{};
    case OFP_VERSION_3:
        return OXMRange{BytePtr(this) + 20, offset<Big16>(18)};
    default:
        return OXMRange{BytePtr(this) + UnpaddedSizeWithMatchHeader,
                        matchLength_};
    }
}

PacketInBuilder::PacketInBuilder(const PacketIn *msg)
{
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

UInt32 PacketInBuilder::send(Writable *channel)
{
    UInt8 version = channel->version();
    switch (version) {
    case OFP_VERSION_1:
        return sendV1(channel);
    case OFP_VERSION_2:
        return sendV2(channel);
    case OFP_VERSION_3:
        return sendV3(channel);
    default:
        break;
    }

    // Construct the match. CHECK: all three fields are in every packet?
    match_.clear();
    match_.add(OFB_IN_PORT{inPort_});
    match_.add(OFB_IN_PHY_PORT{inPhyPort_});
    match_.add(OFB_METADATA{metadata_});
    
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
    hdr.setVersion(version);
    hdr.setType(PacketIn::type());
    hdr.setLength(UInt16_narrow_cast(msgLen));
    hdr.setXid(xid);

    // Fill in the match header.
    msg_.matchType_ = OFPMT_OXM;
    msg_.matchLength_ = UInt16_narrow_cast(PacketIn::MatchHeaderSize + match_.size());

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

UInt32 PacketInBuilder::sendV1(Writable *channel)
{
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

UInt32 PacketInBuilder::sendV2(Writable *channel)
{
    assert(channel->version() == OFP_VERSION_2);

    UInt32 xid = channel->nextXid();

    // FIXME - Unimplemented
    
    return xid;
}

UInt32 PacketInBuilder::sendV3(Writable *channel)
{
    assert(channel->version() == OFP_VERSION_3);

    UInt32 xid = channel->nextXid();

    // FIXME - Unimplemented
    
    return xid;
}

