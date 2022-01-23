// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/byteorder.h"

#include "ofp/unittest.h"

using namespace ofp;

TEST(byteorder, IsHostBigEndian) {
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

TEST(byteorder, SwapTwoBytes) {
  EXPECT_EQ(0x0201, detail::SwapTwoBytes(0x0102));
  EXPECT_EQ(0xf2f4, detail::SwapTwoBytes(0xf4f2));
}

TEST(byteorder, SwapFourBytes) {
  EXPECT_EQ(0x04030201, detail::SwapFourBytes(0x01020304));
  EXPECT_EQ(0xf2f4f8fa, detail::SwapFourBytes(0xfaf8f4f2));
}

TEST(byteorder, SwapEightBytes) {
  EXPECT_EQ(0x0807060504030201, detail::SwapEightBytes(0x0102030405060708));
  EXPECT_EQ(0xf2f4f8fafcfef0f4, detail::SwapEightBytes(0xf4f0fefcfaf8f4f2));
}

TEST(byteorder, Big8) {
  Big8 a{2};
  EXPECT_EQ(0, std::memcmp(&a, "\2", 1));
  EXPECT_EQ(2, a);

  a = 5;
  EXPECT_EQ(0, std::memcmp(&a, "\5", 1));
  EXPECT_EQ(5, a);
}

TEST(byteorder, Big16) {
  Big16 a{0x0201U};
  EXPECT_EQ(0, std::memcmp(&a, "\2\1", 2));
  EXPECT_EQ(0x0201U, a);

  a = 0x0102U;
  EXPECT_EQ(0, std::memcmp(&a, "\1\2", 2));
  EXPECT_EQ(0x0102U, a);
}

TEST(byteorder, Big32) {
  Big32 a{0x04050607};
  EXPECT_EQ(0, std::memcmp(&a, "\4\5\6\7", 4));
  EXPECT_EQ(0x04050607, a);

  a = 0x01020304UL;
  EXPECT_EQ(0, std::memcmp(&a, "\1\2\3\4", 4));
  EXPECT_EQ(0x01020304UL, a);
}

TEST(byteorder, Big64) {
  Big64 a{0x0807060504030201ULL};
  EXPECT_EQ(0, std::memcmp(&a, "\10\7\6\5\4\3\2\1", 8));
  EXPECT_EQ(0x0807060504030201ULL, a);

  a = 0x0102030405060708ULL;
  EXPECT_EQ(0, std::memcmp(&a, "\1\2\3\4\5\6\7\10", 8));
  EXPECT_EQ(0x0102030405060708ULL, a);
}

TEST(byteorder, BigEnum8) {
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

TEST(byteorder, BigEnum16) {
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

TEST(byteorder, BigEnum32) {
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

TEST(byteorder, BigEnum) {
  // Plain unadorned enum.
  enum Foo : int { a, b, c };
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

// Make sure we can use BigEndianFromNative in a constexpr.
constexpr UInt32 ConvertToBigEndian(UInt32 value) {
  return BigEndianFromNative(value);
}

TEST(byteorder, BigEndianFromNative) {
  UInt32 val = ConvertToBigEndian(0x01020304U);
  EXPECT_EQ(0, std::memcmp(&val, "\1\2\3\4", 4));
}

TEST(byteorder, BigEndianToNative) {
  auto a = BigEndianFromNative(0x80000000UL);
  auto b = BigEndianToNative(a);

  EXPECT_EQ(0x80000000UL, b);
}

TEST(byteorder, equals) {
  Big16 a{3};
  Big16 b{3};

  EXPECT_TRUE(a == b);
}

TEST(byteorder, operator_not) {
  Big16 a{0x1122};
  Big16 b{0};

  EXPECT_FALSE(!a);
  EXPECT_TRUE(!b);
}

TEST(byteorder, Big24) {
  Big24 a{0x04050607};
  EXPECT_EQ(0, std::memcmp(&a, "\5\6\7", 3));
  EXPECT_EQ(0x050607, a);

  a = 0x01020304UL;
  EXPECT_EQ(0, std::memcmp(&a, "\2\3\4", 3));
  EXPECT_EQ(0x020304UL, a);

  Big24 b;
  EXPECT_EQ(0, std::memcmp(&b, "\0\0\0", 3));
  EXPECT_EQ(0, b);
}

TEST(byteorder, fromBytes) {
  Big32 val32 = 0x11111111;
  Big64 val64 = 0x1111111122222222;

  auto a = Big32::fromBytes(BytePtr(&val32), 0);
  EXPECT_EQ(0, a);

  auto b = Big32::fromBytes(BytePtr(&val32), 4);
  EXPECT_EQ(0x11111111, b);

  auto c = Big32::fromBytes(BytePtr(&val64), 8);
  EXPECT_EQ(0x22222222, c);
}

TEST(byteorder, Big16_unaligned) {
  Big32 x = 0x01020304;
  const UInt8 *p = BytePtr(&x);
  EXPECT_EQ(0x0203, Big16_unaligned(p + 1));
}

TEST(byteorder, Big32_unaligned) {
  Big64 x = 0x0102030405060708;
  const UInt8 *p = BytePtr(&x);
  EXPECT_EQ(0x02030405, Big32_unaligned(p + 1));
}

// Test NativeTypeOf construct.

static_assert(std::is_same<NativeTypeOf<Big64>::type, UInt64>::value,
              "Unexpected type");
static_assert(std::is_same<NativeTypeOf<Big32>::type, UInt32>::value,
              "Unexpected type");
static_assert(std::is_same<NativeTypeOf<Big16>::type, UInt16>::value,
              "Unexpected type");
static_assert(std::is_same<NativeTypeOf<Big8>::type, UInt8>::value,
              "Unexpected type");
static_assert(std::is_same<NativeTypeOf<Big24>::type, UInt32>::value,
              "Unexpected type");
static_assert(std::is_same<NativeTypeOf<UInt32>::type, UInt32>::value,
              "Unexpected type");

struct Foo {};

static_assert(std::is_same<NativeTypeOf<Foo>::type, Foo>::value,
              "Unexpected type");

struct Bar {
  using NativeType = Foo;
};

static_assert(std::is_same<NativeTypeOf<Bar>::type, Foo>::value,
              "Unexpected type");
