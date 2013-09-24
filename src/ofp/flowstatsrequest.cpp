//  ===== ---- ofp/flowstatsrequest.cpp --------------------*- C++ -*- =====  //
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
/// \brief Implements FlowStatsRequest and FlowStatsRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

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