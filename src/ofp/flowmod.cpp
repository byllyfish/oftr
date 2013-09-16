#include "ofp/flowmod.h"
#include "ofp/writable.h"
#include "ofp/originalmatch.h"

namespace ofp { // <namespace ofp>


Match FlowMod::match() const
{
    assert(validateLength(header_.length()));

    UInt16 type = matchType_;

    if (type == OFPMT_OXM) {
        OXMRange range{BytePtr(this) + UnpaddedSizeWithMatchHeader, matchLength_ - MatchHeaderSize};
        return Match{range};

    } else if (type == OFPMT_STANDARD) {
        assert(matchLength_ == deprecated::OFPMT_STANDARD_LENGTH);
        const deprecated::StandardMatch *stdMatch = reinterpret_cast<const deprecated::StandardMatch *>(BytePtr(this) + SizeWithoutMatchHeader);
        return Match{stdMatch};

    } else {
        log::debug("Unknown matchType:", type);
        return Match{OXMRange{nullptr, 0}};
    }
}

InstructionRange FlowMod::instructions() const
{
    size_t offset = PadLength(SizeWithoutMatchHeader + matchLength_);
    assert(header_.length() >= offset);

    return InstructionRange{ByteRange{BytePtr(this) + offset, header_.length() - offset}}; 
}


bool FlowMod::validateLength(size_t length) const
{
    if (length < UnpaddedSizeWithMatchHeader) {
        log::debug("FlowMod too small", length);
        return false;
    }

    // Check the match length.
    UInt16 matchLen = matchLength_;
    if (matchLen < MatchHeaderSize) {
        log::debug("Match header size too small", matchLen);
        return false;
    }

    if (length < SizeWithoutMatchHeader + matchLen) {
        log::debug("FlowMod too small with match", length);
        return false;
    }

    UInt16 matchType = matchType_;
    if (matchType == OFPMT_OXM) {
        OXMRange range{BytePtr(this) + UnpaddedSizeWithMatchHeader, matchLen - MatchHeaderSize};
        if (!range.validateLength()) {
            log::debug("FlowMod OXM list has length mismatch");
            return false;
        }

    } else if (matchType == OFPMT_STANDARD) {
        if (matchLen != deprecated::OFPMT_STANDARD_LENGTH) {
            log::debug("FlowMod StandardMatch unexpected length", matchLen);
            return false;
        }
    }

    return true;
}


UInt32 FlowModBuilder::send(Writable *channel)
{
    UInt8 version = channel->version();
    if (version <= OFP_VERSION_1) {
        return sendOriginal(channel);
    } else if (version == OFP_VERSION_2) {
        return sendStandard(channel);
    }

    // Calculate length of FlowMod message up to end of match section. Then
    // pad it to a multiple of 8 bytes.
    size_t msgMatchLen = FlowMod::UnpaddedSizeWithMatchHeader + match_.size();
    size_t msgMatchLenPadded = PadLength(msgMatchLen);

    // Calculate length of instruction section. Then, pad it to a multiple
    // of 8 bytes.
    size_t instrLen = instructions_.size();
    size_t instrLenPadded = PadLength(instrLen);

    // Calculate the total FlowMod message length.
    size_t msgLen = msgMatchLenPadded + instrLenPadded;

    // Fill in the message header.
    UInt32 xid = channel->nextXid();
    Header &hdr = msg_.header_;
    hdr.setVersion(version);
    hdr.setType(FlowMod::type());
    hdr.setLength(UInt16_narrow_cast(msgLen));
    hdr.setXid(xid);

    // Fill in the match header.
    msg_.matchType_ = OFPMT_OXM;
    msg_.matchLength_ = UInt16_narrow_cast(FlowMod::MatchHeaderSize + match_.size());

    // Write the message with padding in the correct spots.
    Padding<8> pad;
    channel->write(&msg_, FlowMod::UnpaddedSizeWithMatchHeader);
    channel->write(match_.data(), match_.size());
    channel->write(&pad, msgMatchLenPadded - msgMatchLen);
    channel->write(instructions_.data(), instructions_.size());
    channel->flush();

    return xid;
}

UInt32 FlowModBuilder::sendStandard(Writable *channel)
{
    UInt8 version = channel->version();
    assert(version >= OFP_VERSION_2);

    deprecated::StandardMatch stdMatch{match_.toRange()};

    size_t msgMatchLen = FlowMod::SizeWithoutMatchHeader + sizeof(stdMatch);
    size_t instrLen = instructions_.size();
    size_t instrLenPadded = PadLength(instrLen);
    size_t msgLen = msgMatchLen + instrLenPadded;

    UInt32 xid = channel->nextXid();
    Header &hdr = msg_.header_;
    hdr.setVersion(version);
    hdr.setType(FlowMod::type());
    hdr.setLength(UInt16_narrow_cast(msgLen));
    hdr.setXid(xid);

    channel->write(&msg_, FlowMod::SizeWithoutMatchHeader);
    channel->write(&stdMatch, sizeof(stdMatch));
    channel->write(instructions_.data(), instrLen);
    channel->flush();

    return xid;
}


UInt32 FlowModBuilder::sendOriginal(Writable *channel)
{
    UInt8 version = channel->version();
    assert(version <= OFP_VERSION_1);

    deprecated::OriginalMatch origMatch{match_.toRange()};
    ActionRange actions = instructions_.toActions();
    
    UInt16 msgLen = 72;
    if (actions.size() > 0) {
        msgLen += actions.writeSize(channel);
    }

    UInt32 xid = channel->nextXid();
    Header &hdr = msg_.header_;
    hdr.setVersion(version);
    hdr.setType(FlowMod::type());
    hdr.setLength(msgLen);
    hdr.setXid(xid);

    if (msg_.tableId_ != 0) {
        log::info("FlowModBuilder: tableId not supported in version 1.");
        msg_.tableId_ = 0;
    }

    channel->write(&msg_, sizeof(Header));
    channel->write(&origMatch, sizeof(origMatch));
    channel->write(&msg_.cookie_, 8);
    channel->write(&msg_.tableId_, 12);
    channel->write(BytePtr(&msg_.outPort_) + 2, 2);
    channel->write(&msg_.flags_, 2);

    if (actions.size() > 0) {
        actions.write(channel);
    }

    channel->flush();
    
    return xid;
}

} // </namespace ofp>