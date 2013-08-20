#include "ofp/flowmod.h"
#include "ofp/writable.h"


const ofp::FlowMod *ofp::FlowMod::cast(const Message *message)
{
	return message->cast<FlowMod>();
}


ofp::Match ofp::FlowMod::match() const
{
    assert(validateLength(header_.length()));

    UInt16 type = matchType_;

    if (type == OFPMT_OXM) {
        OXMRange range{BytePtr(this) + UnpaddedSizeWithMatchHeader, matchLength_};
        return Match{range};

    } else if (type == OFPMT_STANDARD) {
        const deprecated::StandardMatch *stdMatch = reinterpret_cast<const deprecated::StandardMatch *>(BytePtr(this) + SizeWithoutMatchHeader);
        return Match{stdMatch};

    } else {
        log::debug("Unknown matchType:", type);
        return Match{OXMRange{nullptr, 0}};
    }
}


bool ofp::FlowMod::validateLength(size_t length) const
{
    if (length < UnpaddedSizeWithMatchHeader) {
        log::debug("FlowMod too small");
        return false;
    }

    // Check the match length.
    UInt16 matchLen = matchLength_;
    if (length < UnpaddedSizeWithMatchHeader + matchLen) {
        log::debug("FlowMod too small with match");
        return false;
    }

    UInt16 matchType = matchType_;
    if (matchType == OFPMT_OXM) {
        OXMRange range{BytePtr(this) + UnpaddedSizeWithMatchHeader, matchLen};
        if (!range.validateLength()) {
            log::debug("FlowMod OXM list has length mismatch");
            return false;
        }

    } else if (matchType == OFPMT_STANDARD) {
        if (matchLen != deprecated::OFPMT_STANDARD_LENGTH) {
            log::debug("FlowMod StandardMatch unexpected length");
            return false;
        }
    }

    return true;
}


ofp::UInt32 ofp::FlowModBuilder::send(Writable *channel)
{
    UInt8 version = channel->version();
    if (version <= OFP_VERSION_2) {
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
    hdr.setType(FlowMod::Type);
    hdr.setLength(UInt16_narrow_cast(msgLen));
    hdr.setXid(xid);

    // Fill in the match header.
    msg_.matchType_ = OFPMT_OXM;
    msg_.matchLength_ = UInt16_narrow_cast(match_.size());

    // Write the message with padding in the correct spots.
    Padding<8> pad;
    channel->write(&msg_, FlowMod::UnpaddedSizeWithMatchHeader);
    channel->write(match_.data(), match_.size());
    channel->write(&pad, msgMatchLenPadded - msgMatchLen);
    channel->write(instructions_.data(), instructions_.size());

    return xid;
}

ofp::UInt32 ofp::FlowModBuilder::sendStandard(Writable *channel)
{
    UInt8 version = channel->version();
    assert(version <= OFP_VERSION_2);

    deprecated::StandardMatch stdMatch{match_.toRange()};

    size_t msgMatchLen = FlowMod::SizeWithoutMatchHeader + sizeof(stdMatch);
    size_t instrLen = instructions_.size();
    size_t instrLenPadded = PadLength(instrLen);
    size_t msgLen = msgMatchLen + instrLenPadded;

    UInt32 xid = channel->nextXid();
    Header &hdr = msg_.header_;
    hdr.setVersion(version);
    hdr.setType(FlowMod::Type);
    hdr.setLength(UInt16_narrow_cast(msgLen));
    hdr.setXid(xid);

    channel->write(&msg_, FlowMod::SizeWithoutMatchHeader);
    channel->write(&stdMatch, sizeof(stdMatch));
    channel->write(instructions_.data(), instrLen);

    return xid;
}