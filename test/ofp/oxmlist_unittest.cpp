#include "ofp/unittest.h"
#include "ofp/oxmlist.h"
#include "ofp/oxmvalue.h"

using namespace ofp;

namespace {

using OFB_InPort = OXMValue<0x8000, 0, Big16, 16, false>;
using OFB_VlanVid = OXMValue<0x8000, 6, Big16, 13, true>;
using OFB_TCPSrcPort = OXMValue<0x8000, 19, Big16, 16, false>;


const char *buffer = "8000 0002 012c"
					 "8000 0c02 00c8"
					 "8000 2602 0064";
}

TEST(oxmlist, test)
{
	OXMList list;
	list.add(OFB_InPort{300});
	list.add(OFB_VlanVid{200});
	list.add(OFB_TCPSrcPort{100});
	EXPECT_EQ(18, list.size());
	
	std::string buf = HexToRawData(buffer);
	EXPECT_EQ(buf.size(), list.size());
	EXPECT_EQ(0, std::memcmp(list.data(), buf.data(), list.size()));
	
	OXMRange r{buf.data(), buf.size()};
	OXMRange s = list.toRange();
	EXPECT_EQ(r, s);
	
	OXMList list2{r};
	EXPECT_EQ(list, list2);
	
	OXMList list3{list2};
	EXPECT_EQ(list, list3);
}
