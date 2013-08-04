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
	EXPECT_EQ(231, UInt16_cast(b));  // result is different from static_cast.
	EXPECT_EQ(65486, UInt16_narrow_cast(b << 1));
}

TEST(types, Unsigned_difference) 
{
	UInt32 a = 6;
	UInt32 b = 5;

	EXPECT_EQ(1, Unsigned_difference(a, b));
	EXPECT_EQ(1, a - b);

	EXPECT_EQ(0, Unsigned_difference(b, a));
	EXPECT_NE(0, b - a);

	// Doesn't compile; UInt32 and UInt16 are different types.
	// UInt16 c = 2;
	// EXPECT_EQ(4, Unsigned_difference(a, c));

	// Doesn't compile; Unsigned_difference only accepts unsigned types.
	// int x = 10;
	// int y = 9;
	// EXPECT_EQ(1, Unsigned_difference(x, y));
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

TEST(types, RawDataToHex2)
{
	std::string s{"abcdef"};
	EXPECT_EQ("61-62-63-64-65-66", RawDataToHex(s.data(), s.length(), '-'));
	EXPECT_EQ("", RawDataToHex(s.data(), 0, '-'));

	EXPECT_EQ("6162 6364 6566", RawDataToHex(s.data(), s.length(), ' ', 2));
	EXPECT_EQ("616263 646566", RawDataToHex(s.data(), s.length(), ' ', 3));
	EXPECT_EQ("61626364 6566", RawDataToHex(s.data(), s.length(), ' ', 4));
	EXPECT_EQ(" 61 62 63 64 65 66", RawDataToHex(s.data(), s.length(), ' ', 0));
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

TEST(types, HexToRawData2)
{
	auto raw = HexToRawData("01-02:03\n04 z 05 = 06 _ 07 08 09");
	EXPECT_EQ(9, raw.length());
	EXPECT_EQ(0, std::memcmp(raw.data(), "\1\2\3\4\5\6\7\x08\x09", 9));
}


TEST(types, IsMemFilled) 
{
	EXPECT_TRUE(IsMemFilled("", 0, 'a'));
	EXPECT_TRUE(IsMemFilled(nullptr, 0, 'a'));

	EXPECT_TRUE(IsMemFilled("a", 1, 'a'));
	EXPECT_TRUE(IsMemFilled("aa", 2, 'a'));
	EXPECT_TRUE(IsMemFilled("aaa", 3, 'a'));

	EXPECT_FALSE(IsMemFilled("b", 1, 'a'));
	EXPECT_FALSE(IsMemFilled("ab", 2, 'a'));
	EXPECT_FALSE(IsMemFilled("aba", 3, 'a'));
}

