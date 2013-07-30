#include <gtest/gtest.h>
#include "ofp/types.h"

using namespace ofp;


TEST(types, Unsigned_cast)
{
	int a[] = { 1, 2, 3 };
	
	size_t diff1 = Unsigned_cast(&a[3] - &a[0]);
	EXPECT_EQ(3UL, diff1);
	
	size_t diff2 = Unsigned_cast(&a[0] - &a[3]); 	// huge number
	EXPECT_LT(3, diff2);
	
	std::int16_t b = -25;
	EXPECT_EQ(65511, Unsigned_cast(b));
	EXPECT_EQ(4294967246, Unsigned_cast(b << 1));
	
	std::int8_t c = -25;
	EXPECT_EQ(231, Unsigned_cast(c));
	EXPECT_EQ(4294967246, Unsigned_cast(c << 1));	
}

TEST(types, UInt16_cast)
{
	std::int16_t a = -25;
	EXPECT_EQ(65511, UInt16_cast(a));
	EXPECT_EQ(65486, UInt16_narrow_cast(a << 1));
	
	std::int8_t b = -25;
	EXPECT_EQ(231, UInt16_cast(b));  // result different from static_cast.
	EXPECT_EQ(65486, UInt16_narrow_cast(b << 1));
}

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

TEST(types, ReadMemory)
{
	const char *buf = "abcdef";
	UInt32 value = ReadMemory<UInt32>(buf);
	EXPECT_EQ(0, std::memcmp(buf, &value, sizeof(value)));
}
