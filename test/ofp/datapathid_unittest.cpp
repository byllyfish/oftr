#include "ofp/unittest.h"
#include "ofp/datapathid.h"

using namespace ofp;

TEST(datapathid, test) 
{
	DatapathID a;
	EXPECT_HEX("0000 0000 0000 0000", &a, sizeof(a));

	DatapathID b{0x1234, EnetAddress{"00-01-02-03-04-05"}};
	EXPECT_EQ(0x1234, b.implementerDefined());
	EXPECT_EQ(EnetAddress{"00-01-02-03-04-05"}, b.macAddress());
	EXPECT_HEX("1234 00 01 02 03 04 05", &b, sizeof(b));

	EXPECT_EQ("0000-0000-0000-0000", a.toString());
	EXPECT_EQ("1234-0001-0203-0405", b.toString());

	EXPECT_LT(a, b);
	EXPECT_NE(a, b);

	DatapathID c{b};
	EXPECT_EQ(b, c);
}
