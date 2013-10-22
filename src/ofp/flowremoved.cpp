#include "ofp/flowremoved.h"
#include "ofp/message.h"
#include "ofp/writable.h"
#include "ofp/originalmatch.h"

using namespace ofp;

bool FlowRemoved::validateLength(size_t length) const
{
	// FIXME
	return false;
}

FlowRemovedBuilder::FlowRemovedBuilder(const FlowRemoved *msg) : msg_{*msg}
{
	// FIXME copy match...
}


UInt32 FlowRemovedBuilder::send(Writable *channel)
{
	UInt8 version = channel->version();
	if (version == OFP_VERSION_1) {
        return sendOriginal(channel);
    } else if (version == OFP_VERSION_2) {
        return sendStandard(channel);
    }

	UInt32 xid = channel->nextXid();
	size_t msgLen = FlowRemoved::UnpaddedSizeWithMatchHeader + match_.size();

	msg_.header_.setVersion(version);
	msg_.header_.setLength(UInt16_narrow_cast(msgLen));
	msg_.header_.setXid(xid);

	// Fill in the match header.
    msg_.matchType_ = OFPMT_OXM;
    msg_.matchLength_ = UInt16_narrow_cast(FlowRemoved::MatchHeaderSize + match_.size());

	channel->write(&msg_, FlowRemoved::UnpaddedSizeWithMatchHeader);
	channel->write(match_.data(), match_.size());
	channel->flush();

	return xid;
}


UInt32 FlowRemovedBuilder::sendStandard(Writable *channel)
{
    UInt8 version = channel->version();
    assert(version == OFP_VERSION_2);

    deprecated::StandardMatch stdMatch{match_.toRange()};

    size_t msgLen = FlowRemoved::SizeWithoutMatchHeader + sizeof(stdMatch);
    UInt32 xid = channel->nextXid();
    
    msg_.header_.setVersion(version);
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));
    msg_.header_.setXid(xid);

	if (msg_.hardTimeout_ != 0) {
    	log::info("FlowRemovedBuilder: hardTimeout not supported in version 2.");
        msg_.hardTimeout_ = 0;
    }

    channel->write(&msg_, FlowRemoved::SizeWithoutMatchHeader);
    channel->write(&stdMatch, sizeof(stdMatch));
    channel->flush();

    return xid;
}


UInt32 FlowRemovedBuilder::sendOriginal(Writable *channel)
{
    UInt8 version = channel->version();
    assert(version <= OFP_VERSION_1);

    deprecated::OriginalMatch origMatch{match_.toRange()};
    
    constexpr UInt16 msgLen = sizeof(Header) + sizeof(origMatch) + FlowRemoved::SizeWithoutMatchHeader - sizeof(Header);
    static_assert(msgLen == 88, "Unexpected size.");

    UInt32 xid = channel->nextXid();

    msg_.header_.setVersion(version);
    msg_.header_.setLength(msgLen);
    msg_.header_.setXid(xid);

    if (msg_.tableId_ != 0) {
        log::info("FlowRemovedBuilder: tableId not supported in version 1.");
        msg_.tableId_ = 0;
    }

    if (msg_.hardTimeout_ != 0) {
    	log::info("FlowRemovedBuilder: hardTimeout not supported in version 1.");
        msg_.hardTimeout_ = 0;
    }

    channel->write(&msg_, sizeof(Header));
    channel->write(&origMatch, sizeof(origMatch));
    channel->write(&msg_.cookie_, FlowRemoved::SizeWithoutMatchHeader - sizeof(Header));
    channel->flush();

    return xid;
}
