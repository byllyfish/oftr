#include "ofp/unittest.h"
#include "ofp-c/ofpstring.h"



TEST(ofpstring, basic) 
{
	OFPString str = ofpStringNewEmpty();

	EXPECT_EQ(NULL, str.data);
	EXPECT_EQ(0, str.length);
	EXPECT_EQ(0, str.capacity);

	int ret;
	const char *expected = "test test";
	ret = ofpStringSet(&str, expected, strlen(expected));
	EXPECT_TRUE(ret);

	EXPECT_EQ(9, str.length);
	EXPECT_EQ(10, str.capacity);
	EXPECT_EQ(0, strcmp(expected, str.data));

	ofpStringClear(&str);

	EXPECT_EQ(NULL, str.data);
	EXPECT_EQ(0, str.length);
	EXPECT_EQ(0, str.capacity);	
}
