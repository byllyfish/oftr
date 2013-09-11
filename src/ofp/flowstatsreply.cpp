#include "ofp/flowstatsreply.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>


Match FlowStatsReply::match() const
{
    UInt16 type = matchType_;

    if (type == OFPMT_OXM) {
        OXMRange range{BytePtr(this) + UnpaddedSizeWithMatchHeader, matchLength_};
        return Match{range};

    } else if (type == OFPMT_STANDARD) {
        const deprecated::StandardMatch *stdMatch = reinterpret_cast<const deprecated::StandardMatch *>(BytePtr(this) + SizeWithoutMatchHeader);
        return Match{stdMatch};

    } else {
        log::debug("FlowStatsRequest: Unknown matchType:", type);
        return Match{OXMRange{nullptr, 0}};
    }
}


InstructionRange FlowStatsReply::instructions() const
{
	size_t offset = UnpaddedSizeWithMatchHeader + matchLength_;
	assert(length_ >= offset);

	return InstructionRange{ByteRange{BytePtr(this) + offset, length_ - offset}}; 
}


void FlowStatsReplyBuilder::write(Writable *channel)
{
	size_t msgLen = FlowStatsReply::UnpaddedSizeWithMatchHeader + match_.size() + instructions_.size();

	msg_.length_ = UInt16_narrow_cast(msgLen);
	msg_.matchType_ = OFPMT_OXM;
	msg_.matchLength_ = UInt16_narrow_cast(match_.size());

	channel->write(&msg_, FlowStatsReply::UnpaddedSizeWithMatchHeader);
	channel->write(match_.data(), match_.size());
	channel->write(instructions_.data(), instructions_.size());
	channel->flush();
}

} // </namespace ofp>