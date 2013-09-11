#include "ofp/flowstatsrequest.h"
#include "ofp/writable.h"
#include "ofp/multipartrequest.h"

namespace ofp { // <namespace ofp>


const FlowStatsRequest *FlowStatsRequest::cast(const MultipartRequest *req)
{
	return req->body_cast<FlowStatsRequest>();
}

bool FlowStatsRequest::validateLength(size_t length) const
{	
	if (length < sizeof(FlowStatsRequest)) {
		log::debug("FlowStatsRequest: Validation failed.");
		return false;
	}

	return true;
}


Match FlowStatsRequest::match() const
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

void FlowStatsRequestBuilder::write(Writable *channel)
{
	msg_.matchType_ = OFPMT_OXM;
	msg_.matchLength_ = UInt16_narrow_cast(match_.size());

	channel->write(&msg_, FlowStatsRequest::UnpaddedSizeWithMatchHeader);
	channel->write(match_.data(), match_.size());
	channel->flush();
}


} // </namespace ofp>