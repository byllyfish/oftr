#include "ofp/unittest.h"
#include "ofp/oxmrange.h"
#include "ofp/oxmvalue.h"

using namespace ofp;

namespace {

using OFB_InPort = OXMValue<0x8000, 0, Big16, 16, false>;
using OFB_VlanVid = OXMValue<0x8000, 6, Big16, 13, true>;
using OFB_TCPSrcPort = OXMValue<0x8000, 19, Big16, 16, false>;


const char *buffer = "8000 0002 0100"
					 "8000 0c02 FFFF"
					 "8000 2602 0064";
}

TEST(oxmrange, test)
{
	auto buf = HexToRawData(buffer);
	OXMRange range{buf.data(), buf.size()};
	
	UInt16 port = 0;
	for (auto &item : range) {
		switch (item.type())
		{
			case OFB_TCPSrcPort::type():
				port = item.value<OFB_TCPSrcPort>();
				break;
		}
	}
	
	EXPECT_EQ(100, port);
}
