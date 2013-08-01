#include <gtest/gtest.h>
#include "ofp/oxmvalue.h"
#include "ofp/oxmiterator.h"
#include "ofp/oxmrange.h"

using namespace ofp;

namespace {

using ofb_in_port = OXMValue<0x8000, 0, Big16, 16, false>;
using ofb_vlan_vid = OXMValue<0x8000, 6, Big16, 13, true>;
using ofb_tcp_src_port = OXMValue<0x8000, 19, Big16, 16, false>;


const char *buffer = "8000 0002 0100"
					 "8000 0c02 FFFF"
					 "8000 2602 0064";
}

TEST(oxmiterator, test)
{
	auto buf = HexToRawData(buffer);
	
	OXMRange data{buf.data(), buf.size()};
	OXMIterator begin = data.begin();
	OXMIterator end = data.end();
	
	for (auto iter = begin; iter != end; ++iter) {
		auto item = *iter;
		switch (item.type())
		{
			case ofb_tcp_src_port::type():
				auto srcPort = item.value<ofb_tcp_src_port>();
				EXPECT_EQ(100, srcPort);
				break;
		}
	}
}
