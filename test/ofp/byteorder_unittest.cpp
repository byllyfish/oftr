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

TEST(byteorder, big8)
{
	big8 a;
	a = 0x05;
	
	EXPECT_EQ(0, std::memcmp(&a, "\5", 1));
}

TEST(byteorder, big16)
{
	big16 a;
	a = 0x0102U;

	EXPECT_EQ(0, std::memcmp(&a, "\1\2", 2));
}

TEST(byteorder, big32)
{
	big32 a;
	a = 0x01020304UL;
	
	EXPECT_EQ(0, std::memcmp(&a, "\1\2\3\4", 2));
}

TEST(byteorder, big64)
{
	big64 a;
	a = 0x0102030405060708ULL;
	
	EXPECT_EQ(0, std::memcmp(&a, "\1\2\3\4\5\6\7\10", 2));
}

TEST(byteorder, BigEnum8)
{
	// Test enum class of type uint16_t.
	enum class Foo8 : uint8_t { a, b, c };
	big<Foo8> x;
	
	static_assert(sizeof(x) == sizeof(uint8_t), "Unexpected enum size.");
	
	x = Foo8::a;
	EXPECT_EQ(Foo8::a, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0", 1));
	
	x = Foo8::b;
	EXPECT_EQ(Foo8::b, x);
	EXPECT_EQ(0, std::memcmp(&x, "\1", 1));
	
	x = Foo8::c;
	EXPECT_EQ(Foo8::c, x);
	EXPECT_EQ(0, std::memcmp(&x, "\2", 1));
	
}

TEST(byteorder, BigEnum16)
{
	// Test enum class of type uint16_t.
	enum class Foo16 : uint16_t { a, b, c };
	big<Foo16> x;
	
	static_assert(sizeof(x) == sizeof(uint16_t), "Unexpected enum size.");
	
	x = Foo16::a;
	EXPECT_EQ(Foo16::a, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0\0", 2));
	
	x = Foo16::b;
	EXPECT_EQ(Foo16::b, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0\1", 2));
	
	x = Foo16::c;
	EXPECT_EQ(Foo16::c, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0\2", 2));
}

TEST(byteorder, BigEnum32)
{
	// Test enum class of type uint32_t.
	enum class Foo32 : uint32_t { a, b, c };
	big<Foo32> x;
	
	static_assert(sizeof(x) == sizeof(uint32_t), "Unexpected enum size.");
	
	x = Foo32::a;
	EXPECT_EQ(Foo32::a, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0\0\0\0", 4));
	
	x = Foo32::b;
	EXPECT_EQ(Foo32::b, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0\0\0\1", 4));
	
	x = Foo32::c;
	EXPECT_EQ(Foo32::c, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0\0\0\2", 4));
}

TEST(byteorder, BigEnum)
{
	// Plain unadorned enum.
	enum Foo { a, b, c };
	big<Foo> x;
	
	static_assert(sizeof(x) == sizeof(int), "Unexpected enum size.");
	static_assert(sizeof(int) == 4, "Unexpected int size.");
	
	x = a;
	EXPECT_EQ(a, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0\0\0\0", sizeof(int)));
	
	x = b;
	EXPECT_EQ(b, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0\0\0\1", sizeof(int)));
	
	x = c;
	EXPECT_EQ(c, x);
	EXPECT_EQ(0, std::memcmp(&x, "\0\0\0\2", sizeof(int)));
}

TEST(byteorder, big8_unaligned)
{
	using big8_unaligned = big_unaligned<uint8_t>;
	
	uint8_t buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	uint8_t val[] = { 0x00, 0x01, 0x02, 0x03, 
						0x04, 0x05, 0x06, 0x07,
						0x08, 0x09 };
						
	for (unsigned i = 0; i < ArrayLength(val); ++i) {
		big8_unaligned *p = reinterpret_cast<big8_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = ~val[i];
		// Be careful about promotion from uint8_t to int.
		EXPECT_EQ(uint8_cast(~val[i]), *p);
		big8 temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, uint8_cast(~val[i]));
		*p = ~*p;
	}
}

TEST(byteorder, big16_unaligned)
{
	using big16_unaligned = big_unaligned<uint16_t>;
	
	uint8_t buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	uint16_t val[] = { 0x0001, 0x0102, 0x0203, 0x0304, 
						0x0405, 0x0506, 0x0607, 0x0708,
						0x0809, 0x090a };
						
	for (unsigned i = 0; i < ArrayLength(val); ++i) {
		big16_unaligned *p = reinterpret_cast<big16_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = ~val[i];
		// Be careful about promotion from uint16_t to int.
		EXPECT_EQ(uint16_cast(~val[i]), *p);
		big16 temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, uint16_cast(~val[i]));
		*p = ~*p;
	}
}


TEST(byteorder, big32_unaligned)
{
	using big32_unaligned = big_unaligned<uint32_t>;
	
	uint8_t buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	uint32_t val[] = { 0x00010203, 0x01020304, 0x02030405, 0x03040506, 
						0x04050607, 0x05060708, 0x06070809, 0x0708090a,
						0x08090a0b, 0x090a0b0c };
						
	for (unsigned i = 0; i < ArrayLength(val); ++i) {
		big32_unaligned *p = reinterpret_cast<big32_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = ~val[i];
		EXPECT_EQ(~val[i], *p);
		big32 temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, ~val[i]);
		*p = ~*p;
	}
}

TEST(byteorder, big64_unaligned)
{
	using big64_unaligned = big_unaligned<uint64_t>;
	
	uint8_t buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	uint64_t val[] = { 0x0001020304050607, 0x0102030405060708, 0x0203040506070809, 0x030405060708090a, 
						0x0405060708090a0b, 0x05060708090a0b0c };
						
	for (unsigned i = 0; i < ArrayLength(val); ++i) {
		big64_unaligned *p = reinterpret_cast<big64_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = ~val[i];
		EXPECT_EQ(~val[i], *p);
		big64 temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, ~val[i]);
		*p = ~*p;
	}
}

TEST(byteorder, BigEnum32_Unaligned)
{
	enum Foo : uint32_t { a, b, c };
	
	using Foo_unaligned = big_unaligned<Foo>;

	uint8_t buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	uint32_t val[] = { 0x00010203, 0x01020304, 0x02030405, 0x03040506, 
						0x04050607, 0x05060708, 0x06070809, 0x0708090a,
						0x08090a0b, 0x090a0b0c };
	
	for (unsigned i = 0; i <= 9; ++i) {
		Foo_unaligned *p = reinterpret_cast<Foo_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = static_cast<Foo>(~val[i]);
		EXPECT_EQ(~val[i], *p);
		big<Foo> temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, ~val[i]);
		*p = static_cast<Foo>(~*p);
	}
}
