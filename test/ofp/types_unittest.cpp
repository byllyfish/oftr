#include <gtest/gtest.h>
#include "ofp/types.h"

using namespace ofp::sys;

TEST(types, ArrayLength)
{
	int a[] = { 1, 2, 3 };
	EXPECT_EQ(3, ArrayLength(a));
}

TEST(types, RawDataToHex)
{
	std::string s{"abcdef"};
	EXPECT_EQ("616263646566", RawDataToHex(s.data(), s.length()));
	EXPECT_EQ("", RawDataToHex(s.data(), 0));
}

TEST(types, HexToRawData) 
{
	char buf[7];
	EXPECT_EQ(6, HexToRawData("616263646566", buf, sizeof(buf)));
	EXPECT_EQ(0, std::memcmp(buf, "abcdef\0", 7));
	
	EXPECT_EQ(7, HexToRawData("0102 0304 05 0607", buf, sizeof(buf)));
	EXPECT_EQ(0, std::memcmp(buf, "\1\2\3\4\5\6\7", 7));
	
	EXPECT_EQ(7, HexToRawData("01-02:03\n04 z 05 = 06 _ 07 08 09", buf, sizeof(buf)));
	EXPECT_EQ(0, std::memcmp(buf, "\1\2\3\4\5\6\7", 7));
	
	EXPECT_EQ(2, HexToRawData("aa bb c", buf, sizeof(buf)));
	EXPECT_EQ(0, std::memcmp(buf, "\xaa\xbb\0\0\0\0\0", 7));
}

