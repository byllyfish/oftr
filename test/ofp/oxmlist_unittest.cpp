#include "ofp/unittest.h"
#include "ofp/oxmlist.h"
#include "ofp/oxmvalue.h"
#include "ofp/oxmfields.h"
#include "ofp/log.h"

using namespace ofp;

namespace {

constexpr OXMInternalID cast(int n) { return static_cast<OXMInternalID>(n); }

using OFB_InPort = OXMValue<cast(0), 0x8000, 0, Big16, 2, false>;
using OFB_VlanVid = OXMValue<cast(1), 0x8000, 6, Big16, 2, true>;
using OFB_TCPSrcPort = OXMValue<cast(2), 0x8000, 19, Big16, 2, false>;


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

TEST(oxmlist, mpls) 
{
	OXMList list;

	OXMType t = OFB_MPLS_LABEL::type();
	EXPECT_HEX("80004404", &t, sizeof(t));

    list.add(OFB_MPLS_LABEL{2});
    list.add(OFB_MPLS_TC{1});
    
    EXPECT_EQ(13, list.size());
    EXPECT_HEX("80004404-00000002 80004601-01", list.data(), list.size());
}
