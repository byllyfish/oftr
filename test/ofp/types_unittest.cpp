// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/types.h"

#include "ofp/unittest.h"

using namespace ofp;

TEST(types, Unsigned_cast) {
  int a[] = {1, 2, 3};

  size_t diff1 = Unsigned_cast(&a[3] - &a[0]);
  EXPECT_EQ(3UL, diff1);

  size_t diff2 = Unsigned_cast(&a[0] - &a[3]);  // huge number
  EXPECT_LT(3, diff2);

  std::int16_t b = -25;
  EXPECT_EQ(65511, Unsigned_cast(b));
  EXPECT_EQ(4294967246, Unsigned_cast(b * 2));

  std::int8_t c = -25;
  EXPECT_EQ(231, Unsigned_cast(c));
  EXPECT_EQ(4294967246, Unsigned_cast(c * 2));
}

TEST(types, UInt16_cast) {
  std::int16_t a = -25;
  EXPECT_EQ(65511, UInt16_cast(a));
  EXPECT_EQ(65486, UInt16_narrow_cast(a * 2));

  std::int8_t b = -25;
  EXPECT_EQ(231, UInt16_cast(b));  // result is different from static_cast.
  EXPECT_EQ(65486, UInt16_narrow_cast(b * 2));
}

TEST(types, ArrayLength) {
  int a[] = {1, 2, 3};
  EXPECT_EQ(3, ArrayLength(a));
}

TEST(types, RawDataToHex) {
  std::string s{"abcdef"};
  EXPECT_EQ("616263646566", RawDataToHex(s.data(), s.length()));
  EXPECT_EQ("", RawDataToHex(s.data(), 0));

  EXPECT_EQ("== hex too big ==", RawDataToHex(s.data(), 0x80000001UL));
}

TEST(types, RawDataToHex_Stream) {
  std::string s{"abcdef"};

  {
    std::string buf;
    llvm::raw_string_ostream oss{buf};

    RawDataToHex(s.data(), 0, oss);
    EXPECT_EQ("", oss.str());
    RawDataToHex(s.data(), s.length(), oss);
    EXPECT_EQ("616263646566", oss.str());
  }

  for (unsigned i = 0; i < 10; ++i) {
    std::string buf;
    llvm::raw_string_ostream oss{buf};

    s += s;
    RawDataToHex(s.data(), s.length(), oss);
    EXPECT_EQ(s.length() * 2, oss.str().size());
  }

  {
    std::string buf;
    llvm::raw_string_ostream oss{buf};

    RawDataToHex(s.data(), 0x80000001UL, oss);
    EXPECT_EQ("== hex too big ==", oss.str());
  }
}

TEST(types, RawDataToHex2) {
  std::string s{"abcdef"};
  EXPECT_EQ("61-62-63-64-65-66", RawDataToHex(s.data(), s.length(), '-'));
  EXPECT_EQ("", RawDataToHex(s.data(), 0, '-'));

  EXPECT_EQ("6162 6364 6566", RawDataToHex(s.data(), s.length(), ' ', 2));
  EXPECT_EQ("616263 646566", RawDataToHex(s.data(), s.length(), ' ', 3));
  EXPECT_EQ("61626364 6566", RawDataToHex(s.data(), s.length(), ' ', 4));
  EXPECT_EQ(" 61 62 63 64 65 66", RawDataToHex(s.data(), s.length(), ' ', 0));

  EXPECT_EQ("== hex too big ==", RawDataToHex(s.data(), 0x80000001UL, ' ', 1));
}

TEST(types, HexToRawData) {
  char buf[7];
  EXPECT_EQ(6, HexToRawData("616263646566", buf, sizeof(buf)));
  EXPECT_EQ(0, std::memcmp(buf, "abcdef\0", 7));

  EXPECT_EQ(7, HexToRawData("0102 0304 05 0607", buf, sizeof(buf)));
  EXPECT_EQ(0, std::memcmp(buf, "\1\2\3\4\5\6\7", 7));

  EXPECT_EQ(
      7, HexToRawData("01-02:03\n04 z 05 = 06 _ 07 08 09", buf, sizeof(buf)));
  EXPECT_EQ(0, std::memcmp(buf, "\1\2\3\4\5\6\7", 7));

  EXPECT_EQ(2, HexToRawData("aa bb c", buf, sizeof(buf)));
  EXPECT_EQ(0, std::memcmp(buf, "\xaa\xbb\0\0\0\0\0", 7));

  // Zero length output buffer.
  EXPECT_EQ(0, HexToRawData("01", buf + sizeof(buf), 0));
}

TEST(types, HexToRawData2) {
  auto raw = HexToRawData("01-02:03\n04 z 05 = 06 _ 07 08 09");
  EXPECT_EQ(9, raw.length());
  EXPECT_EQ(0, std::memcmp(raw.data(), "\1\2\3\4\5\6\7\x08\x09", 9));
}

TEST(types, IsMemFilled) {
  EXPECT_TRUE(IsMemFilled("", 0, 'a'));
  EXPECT_TRUE(IsMemFilled(nullptr, 0, 'a'));

  EXPECT_TRUE(IsMemFilled("a", 1, 'a'));
  EXPECT_TRUE(IsMemFilled("aa", 2, 'a'));
  EXPECT_TRUE(IsMemFilled("aaa", 3, 'a'));

  EXPECT_FALSE(IsMemFilled("b", 1, 'a'));
  EXPECT_FALSE(IsMemFilled("ab", 2, 'a'));
  EXPECT_FALSE(IsMemFilled("aba", 3, 'a'));

  EXPECT_TRUE(IsMemFilled("\xFF\xFF\xFF", 3, '\xFF'));
}

TEST(types, HexToRawData3) {
  bool error = false;

  char buf[7];
  EXPECT_EQ(6, HexToRawData("616263646566", buf, sizeof(buf), &error));
  EXPECT_EQ(0, std::memcmp(buf, "abcdef\0", 7));
  EXPECT_FALSE(error);

  EXPECT_EQ(5, HexToRawData("61626364656", buf, sizeof(buf), &error));
  EXPECT_EQ(0, std::memcmp(buf, "abcde\0", 6));
  EXPECT_TRUE(error);

  EXPECT_EQ(7, HexToRawData("0102 0304 05 0607", buf, sizeof(buf), &error));
  EXPECT_EQ(0, std::memcmp(buf, "\1\2\3\4\5\6\7", 7));
  EXPECT_FALSE(error);

  EXPECT_EQ(7, HexToRawData("01-02:03\n04 z 05 = 06 _ 07 08 09", buf,
                            sizeof(buf), &error));
  EXPECT_EQ(0, std::memcmp(buf, "\1\2\3\4\5\6\7", 7));
  EXPECT_TRUE(error);

  EXPECT_EQ(2, HexToRawData("aa bb c", buf, sizeof(buf), &error));
  EXPECT_EQ(0, std::memcmp(buf, "\xaa\xbb\0\0\0\0\0", 7));
  EXPECT_TRUE(error);
}

TEST(types, RawDataToBase64) {
  std::string s{"abcdef7890abcdef7890abcdef7890"};
  EXPECT_EQ("", RawDataToBase64(s.data(), 0));
  EXPECT_EQ("YQ==", RawDataToBase64(s.data(), 1));
  EXPECT_EQ("YWI=", RawDataToBase64(s.data(), 2));
  EXPECT_EQ("YWJj", RawDataToBase64(s.data(), 3));
  EXPECT_EQ("YWJjZA==", RawDataToBase64(s.data(), 4));
  EXPECT_EQ("YWJjZGVmNzg5MGFiY2RlZjc4OTBhYmNkZWY3ODkw",
            RawDataToBase64(s.data(), s.size()));

  EXPECT_EQ("== base64 too big ==", RawDataToBase64(s.data(), 0x80000001UL));
}

TEST(types, MemCopyMasked) {
  UInt32 a = 0x12345678;
  UInt32 b = 0xFFFF000F;

  UInt32 d;
  MemCopyMasked(&d, &a, &b, sizeof(a));
  EXPECT_EQ(a & b, d);
}

TEST(types, IsPtrAligned) {
  OFP_ALIGNAS(8) UInt64 buf;
  const UInt8 *p = BytePtr(&buf);

  EXPECT_TRUE(IsPtrAligned(p, 8));
  EXPECT_TRUE(IsPtrAligned(p, 4));
  EXPECT_TRUE(IsPtrAligned(p, 2));

  EXPECT_FALSE(IsPtrAligned(p + 1, 8));
  EXPECT_FALSE(IsPtrAligned(p + 1, 4));
  EXPECT_FALSE(IsPtrAligned(p + 1, 2));

  EXPECT_FALSE(IsPtrAligned(p + 2, 8));
  EXPECT_FALSE(IsPtrAligned(p + 2, 4));
  EXPECT_TRUE(IsPtrAligned(p + 2, 2));
}

TEST(types, HexDelimitedToRawData) {
  UInt8 buf[2];

  EXPECT_EQ(1, HexDelimitedToRawData("00", buf, sizeof(buf)));
  EXPECT_HEX("00", buf, 1);
  EXPECT_EQ(2, HexDelimitedToRawData("00:00", buf, sizeof(buf)));
  EXPECT_HEX("00 00", buf, 2);
  EXPECT_EQ(1, HexDelimitedToRawData("fF", buf, sizeof(buf)));
  EXPECT_HEX("FF", buf, 1);
  EXPECT_EQ(2, HexDelimitedToRawData("ff:FF", buf, sizeof(buf)));
  EXPECT_HEX("FF FF", buf, 2);

  EXPECT_EQ(1, HexDelimitedToRawData("9", buf, sizeof(buf)));
  EXPECT_HEX("09", buf, 1);
  EXPECT_EQ(1, HexDelimitedToRawData("d", buf, sizeof(buf)));
  EXPECT_HEX("0d", buf, 1);
  EXPECT_EQ(2, HexDelimitedToRawData("00:3", buf, sizeof(buf)));
  EXPECT_HEX("00 03", buf, 2);
  EXPECT_EQ(2, HexDelimitedToRawData("ff:f", buf, sizeof(buf)));
  EXPECT_HEX("FF 0F", buf, 2);
  EXPECT_EQ(2, HexDelimitedToRawData("f:ff", buf, sizeof(buf)));
  EXPECT_HEX("0F FF", buf, 2);
  EXPECT_EQ(2, HexDelimitedToRawData("f:f", buf, sizeof(buf)));
  EXPECT_HEX("0F 0F", buf, 2);

  EXPECT_EQ(0, HexDelimitedToRawData("", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("00:00:00", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("000", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("00:", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData(":00", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("00:00:", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("00:00:0", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("ff:ff:ff", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("ff:f ", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData(" ff:ff", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("012", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("01:023", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("01::2", buf, sizeof(buf)));

  EXPECT_EQ(0, HexDelimitedToRawData("f:g", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("fg", buf, sizeof(buf)));
  EXPECT_EQ(0, HexDelimitedToRawData("g:f", buf, sizeof(buf)));
}

TEST(types, HexStrictToRawData) {
  UInt8 buf[2];

  EXPECT_EQ(1, HexStrictToRawData("00", buf, sizeof(buf)));
  EXPECT_HEX("00", buf, 1);
  EXPECT_EQ(2, HexStrictToRawData("0000", buf, sizeof(buf)));
  EXPECT_HEX("00 00", buf, 2);
  EXPECT_EQ(1, HexStrictToRawData("fF", buf, sizeof(buf)));
  EXPECT_HEX("FF", buf, 1);
  EXPECT_EQ(2, HexStrictToRawData("ffFF", buf, sizeof(buf)));
  EXPECT_HEX("FF FF", buf, 2);

  EXPECT_EQ(1, HexStrictToRawData("09", buf, sizeof(buf)));
  EXPECT_HEX("09", buf, 1);
  EXPECT_EQ(1, HexStrictToRawData("0d", buf, sizeof(buf)));
  EXPECT_HEX("0d", buf, 1);

  EXPECT_EQ(0, HexStrictToRawData("010203", buf, sizeof(buf)));
  EXPECT_EQ(0, HexStrictToRawData("003", buf, sizeof(buf)));
  EXPECT_EQ(0, HexStrictToRawData("fff", buf, sizeof(buf)));
  EXPECT_EQ(0, HexStrictToRawData("", buf, sizeof(buf)));
  EXPECT_EQ(0, HexStrictToRawData("fg", buf, sizeof(buf)));
  EXPECT_EQ(0, HexStrictToRawData(" ff", buf, sizeof(buf)));
  EXPECT_EQ(0, HexStrictToRawData("ff ", buf, sizeof(buf)));

  EXPECT_EQ(0, HexStrictToRawData("00:00:00", buf, sizeof(buf)));
  EXPECT_EQ(0, HexStrictToRawData("000", buf, sizeof(buf)));
  EXPECT_EQ(0, HexStrictToRawData("00 00", buf, sizeof(buf)));
}

static void watchdogtimer() {
  SetWatchdogTimer(1);
  volatile UInt64 counter = 0;
  while (counter < 0xffffffffffff) {
    ++counter;
  }
}

OFP_BEGIN_IGNORE_USED_BUT_UNUSED

TEST(types_DeathTest, watchdogtimer) {
  // Don't run this test under valgrind; it just spews output.
  if (std::getenv("LIBOFP_VALGRIND"))
    return;

  // This macro generates a warning on clang. [-Wused-but-marked-unused]
  EXPECT_EXIT(watchdogtimer(), ::testing::ExitedWithCode(200),
              "watchdog_timer");
}

OFP_END_IGNORE_USED_BUT_UNUSED
