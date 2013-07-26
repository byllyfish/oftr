#include <gtest/gtest.h>
#include "ofp/byteorder.h"

using namespace ofp::sys;


TEST(byteorder, IsHostBigEndian)
{
	const char *be = "\1\2\3\4";
	const char *le = "\4\3\2\1";
	uint32_t n = 0x01020304;

	if (detail::IsHostBigEndian) {
		EXPECT_FALSE(detail::IsHostLittleEndian);
		EXPECT_EQ(0, std::memcmp(be, &n, 4));
		EXPECT_NE(0, std::memcmp(le, &n, 4));
	} else {
		EXPECT_TRUE(detail::IsHostLittleEndian);
		EXPECT_EQ(0, std::memcmp(le, &n, 4));
		EXPECT_NE(0, std::memcmp(be, &n, 4));
	}
}

TEST(byteorder, SwapTwoBytes)
{
	EXPECT_EQ(0x0201, detail::SwapTwoBytes(0x0102));
	EXPECT_EQ(0xf2f4, detail::SwapTwoBytes(0xf4f2));
}

TEST(byteorder, SwapFourBytes)
{
	EXPECT_EQ(0x04030201, detail::SwapFourBytes(0x01020304));
	EXPECT_EQ(0xf2f4f8fa, detail::SwapFourBytes(0xfaf8f4f2));
}

TEST(byteorder, SwapEightBytes)
{
	EXPECT_EQ(0x0807060504030201, detail::SwapEightBytes(0x0102030405060708));
	EXPECT_EQ(0xf2f4f8fafcfef0f4, detail::SwapEightBytes(0xf4f0fefcfaf8f4f2));
}

TEST(byteorder, ubig16_t)
{
	ubig16_t a;
	a = 0x0102U;

	EXPECT_EQ(0, std::memcmp(&a, "\1\2", 2));
}

TEST(byteorder, ubig32_t)
{
	ubig32_t a;
	a = 0x01020304UL;
	
	EXPECT_EQ(0, std::memcmp(&a, "\1\2\3\4", 2));
}

TEST(byteorder, ubig64_t)
{
	ubig64_t a;
	a = 0x0102030405060708ULL;
	
	EXPECT_EQ(0, std::memcmp(&a, "\1\2\3\4\5\6\7\10", 2));
}



