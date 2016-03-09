// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmtype.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(OXMType, constructor) {
  OXMType a{2, 1, 4};
  EXPECT_EQ(0, std::memcmp(&a, "\0\2\2\4", 4));
  EXPECT_EQ(0x00020204UL, a.oxmNative());
  EXPECT_EQ(2, a.oxmClass());
  EXPECT_EQ(1, a.oxmField());
  EXPECT_EQ(4, a.length());
  EXPECT_FALSE(a.hasMask());

  OXMType b{0x8000, 0xFF, 8};
  EXPECT_EQ(0, std::memcmp(&b, "\x80\x00\xFE\x08", 4));
  EXPECT_EQ(0x8000FE08UL, b.oxmNative());
  EXPECT_EQ(0x8000, b.oxmClass());
  EXPECT_EQ(0x7F, b.oxmField());
  EXPECT_EQ(8, b.length());
  EXPECT_FALSE(b.hasMask());

  OXMType c{0xDEAD, 0x7F, 16};
  EXPECT_EQ(0, std::memcmp(&c, "\xDE\xAD\xFE\x10", 4));
  EXPECT_EQ(0xDEADFE10UL, c.oxmNative());
  EXPECT_EQ(0xDEAD, c.oxmClass());
  EXPECT_EQ(0x7F, c.oxmField());
  EXPECT_EQ(16, c.length());
  EXPECT_FALSE(c.hasMask());
}

TEST(OXMType, use_in_switch_stmt) {
  constexpr OXMType cool{2, 2, 8};

  bool found = false;

  OXMType d{2, 2, 8};
  switch (d) {
    case cool:
      found = true;
      break;

    case OXMType{1, 1, 8}:
    case OXMType{3, 3, 8}:
      break;
  }

  EXPECT_TRUE(found);
}

TEST(OXMType, withMask) {
  constexpr OXMType a{2, 2, 8};
  EXPECT_FALSE(a.hasMask());

  constexpr OXMType b = a.withMask();
  EXPECT_TRUE(b.hasMask());

  EXPECT_NE(a, b);
  EXPECT_EQ(a.oxmClass(), b.oxmClass());
  EXPECT_EQ(a.oxmField(), b.oxmField());
  EXPECT_EQ(2 * a.length(), b.length());
  EXPECT_NE(a.hasMask(), b.hasMask());

  EXPECT_EQ(a.withMask(), b.withMask());

  constexpr OXMType c = b.withoutMask();
  EXPECT_NE(b.hasMask(), c.hasMask());
  EXPECT_EQ(a, c);
  EXPECT_EQ(a.withoutMask(), c.withoutMask());
}

TEST(OXMType, constructFromMemory) {
  const UInt8 raw[] = {0x80, 0xFF, 0xAA, 0x02};

  OXMType type = OXMType::fromBytes(raw);
  EXPECT_EQ(0, std::memcmp(&type, "\x80\xFF\xAA\x02", 4));
  EXPECT_EQ(0x80FFAA02UL, type.oxmNative());
  EXPECT_EQ(0x80FF, type.oxmClass());
  EXPECT_EQ(0xAA >> 1, type.oxmField());
  EXPECT_EQ(2, type.length());
  EXPECT_FALSE(type.hasMask());
}

TEST(OXMType, withMask_Experimenter) {
  constexpr OXMType a{0xFFFF, 2, 8};

  EXPECT_FALSE(a.hasMask());
  EXPECT_TRUE(a.isExperimenter());
  EXPECT_EQ(8, a.length());

  constexpr OXMType b = a.withMask();
  EXPECT_TRUE(b.hasMask());
  EXPECT_TRUE(b.isExperimenter());
  EXPECT_EQ(12, b.length());

  EXPECT_NE(a, b);
  EXPECT_EQ(a, b.withoutMask());
}

TEST(oxmtype, stream) {
  OXMType a{0xFFFF, 2, 8};

  std::ostringstream oss;
  oss << a;
  EXPECT_EQ("FFFF0408", oss.str());
}
