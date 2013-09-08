#include "ofp/unittest.h"
#include "ofp/headeronly.h"

using namespace ofp;

TEST(headeronly, barrierrequestbuilder_v1) 
{
	BarrierRequestBuilder builder;

	MemoryChannel channel{OFP_VERSION_1};
	builder.send(&channel);

	EXPECT_EQ(0x08, channel.size());
	EXPECT_HEX("0112000800000001", channel.data(), channel.size());

	Message message{channel.data(), channel.size()};
	message.transmogrify();
	EXPECT_EQ(BarrierRequest::type(), message.type());

	const BarrierRequest *req = BarrierRequest::cast(&message);
	EXPECT_TRUE(req);

	EXPECT_HEX("0114000800000001", req, message.size());
}

TEST(headeronly, barrierrequestbuilder_v4) 
{
	BarrierRequestBuilder builder;

	MemoryChannel channel{OFP_VERSION_4};
	builder.send(&channel);

	EXPECT_EQ(0x08, channel.size());
	EXPECT_HEX("0414000800000001", channel.data(), channel.size());

	Message message{channel.data(), channel.size()};
	message.transmogrify();
	EXPECT_EQ(BarrierRequest::type(), message.type());

	const BarrierRequest *req = BarrierRequest::cast(&message);
	EXPECT_TRUE(req);

	EXPECT_HEX("0414000800000001", req, message.size());	
}


