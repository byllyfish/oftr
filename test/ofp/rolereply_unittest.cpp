// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/rolereply.h"
#include "ofp/mockchannel.h"

using namespace ofp;

TEST(rolereply, test) {
	RoleRequestBuilder request;
	request.setRole(OFPCR_ROLE_EQUAL);
	request.setGenerationId(1);

	MemoryChannel channel1{OFP_VERSION_4};
	request.send(&channel1);

	EXPECT_HEX("041800180000000100000001000000000000000000000001", channel1.data(), channel1.size());

	Message m1{channel1.data(), channel1.size()};
	const RoleRequest *msg = RoleRequest::cast(&m1);
	ASSERT_TRUE(msg);

	RoleReplyBuilder reply{msg};
	reply.setRole(OFPCR_ROLE_EQUAL);
	reply.setGenerationId(1);

	MemoryChannel channel2{OFP_VERSION_4};
	reply.send(&channel2);

	EXPECT_HEX("041900180000000100000001000000000000000000000001", channel2.data(), channel2.size());
}
