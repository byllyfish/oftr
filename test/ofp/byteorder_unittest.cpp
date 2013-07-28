#include <gtest/gtest.h>
#include "ofp/byteorder.h"

using namespace ofp;


TEST(byteorder, IsHostBigEndian)
{
	const char *be = "\1\2\3\4";
	const char *le = "\4\3\2\1";
	UInt32 n = 0x01020304;

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

TEST(byteorder, Big8)
{
	Big8 a;
	a = 0x05;
	
	EXPECT_EQ(0, std::memcmp(&a, "\5", 1));
}

TEST(byteorder, Big16)
{
	Big16 a;
	a = 0x0102U;

	EXPECT_EQ(0, std::memcmp(&a, "\1\2", 2));
}

TEST(byteorder, Big32)
{
	Big32 a;
	a = 0x01020304UL;
	
	EXPECT_EQ(0, std::memcmp(&a, "\1\2\3\4", 2));
}

TEST(byteorder, Big64)
{
	Big64 a;
	a = 0x0102030405060708ULL;
	
	EXPECT_EQ(0, std::memcmp(&a, "\1\2\3\4\5\6\7\10", 2));
}

TEST(byteorder, BigEnum8)
{
	// Test enum class of type UInt8.
	enum class Foo8 : UInt8 { a, b, c };
	Big<Foo8> x;
	
	static_assert(sizeof(x) == sizeof(UInt8), "Unexpected enum size.");
	
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
	// Test enum class of type UInt16.
	enum class Foo16 : UInt16 { a, b, c };
	Big<Foo16> x;
	
	static_assert(sizeof(x) == sizeof(UInt16), "Unexpected enum size.");
	
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
	// Test enum class of type UInt32.
	enum class Foo32 : UInt32 { a, b, c };
	Big<Foo32> x;
	
	static_assert(sizeof(x) == sizeof(UInt32), "Unexpected enum size.");
	
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
	Big<Foo> x;
	
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

TEST(byteorder, Big8_unaligned)
{
	using Big8_unaligned = Big_unaligned<UInt8>;
	
	UInt8 buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	UInt8 val[] = { 0x00, 0x01, 0x02, 0x03, 
						0x04, 0x05, 0x06, 0x07,
						0x08, 0x09 };
						
	for (unsigned i = 0; i < ArrayLength(val); ++i) {
		Big8_unaligned *p = reinterpret_cast<Big8_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = ~val[i];
		// Be careful about promotion from UInt8 to int.
		EXPECT_EQ(UInt8_cast(~val[i]), *p);
		Big8 temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, UInt8_cast(~val[i]));
		*p = ~*p;
	}
}

TEST(byteorder, Big16_unaligned)
{
	using Big16_unaligned = Big_unaligned<UInt16>;
	
	UInt8 buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	UInt16 val[] = { 0x0001, 0x0102, 0x0203, 0x0304, 
						0x0405, 0x0506, 0x0607, 0x0708,
						0x0809, 0x090a };
						
	for (unsigned i = 0; i < ArrayLength(val); ++i) {
		Big16_unaligned *p = reinterpret_cast<Big16_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = ~val[i];
		// Be careful about promotion from UInt16 to int.
		EXPECT_EQ(UInt16_cast(~val[i]), *p);
		Big16 temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, UInt16_cast(~val[i]));
		*p = ~*p;
	}
}


TEST(byteorder, Big32_unaligned)
{
	using Big32_unaligned = Big_unaligned<UInt32>;
	
	UInt8 buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	UInt32 val[] = { 0x00010203, 0x01020304, 0x02030405, 0x03040506, 
						0x04050607, 0x05060708, 0x06070809, 0x0708090a,
						0x08090a0b, 0x090a0b0c };
						
	for (unsigned i = 0; i < ArrayLength(val); ++i) {
		Big32_unaligned *p = reinterpret_cast<Big32_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = ~val[i];
		EXPECT_EQ(~val[i], *p);
		Big32 temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, ~val[i]);
		*p = ~*p;
	}
}

TEST(byteorder, Big64_unaligned)
{
	using Big64_unaligned = Big_unaligned<UInt64>;
	
	UInt8 buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	UInt64 val[] = { 0x0001020304050607, 0x0102030405060708, 0x0203040506070809, 0x030405060708090a, 
						0x0405060708090a0b, 0x05060708090a0b0c };
						
	for (unsigned i = 0; i < ArrayLength(val); ++i) {
		Big64_unaligned *p = reinterpret_cast<Big64_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = ~val[i];
		EXPECT_EQ(~val[i], *p);
		Big64 temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, ~val[i]);
		*p = ~*p;
	}
}

TEST(byteorder, BigEnum32_Unaligned)
{
	enum Foo : UInt32 { a, b, c };
	
	using Foo_unaligned = Big_unaligned<Foo>;

	UInt8 buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	UInt32 val[] = { 0x00010203, 0x01020304, 0x02030405, 0x03040506, 
						0x04050607, 0x05060708, 0x06070809, 0x0708090a,
						0x08090a0b, 0x090a0b0c };
	
	for (unsigned i = 0; i <= 9; ++i) {
		Foo_unaligned *p = reinterpret_cast<Foo_unaligned *>(&buf[i]);
		EXPECT_EQ(val[i], *p);
		*p = static_cast<Foo>(~val[i]);
		EXPECT_EQ(~val[i], *p);
		Big<Foo> temp;
		std::memcpy(&temp, &buf[i], sizeof(temp));
		EXPECT_EQ(temp, *p);
		EXPECT_EQ(temp, ~val[i]);
		*p = static_cast<Foo>(~*p);
	}
}
