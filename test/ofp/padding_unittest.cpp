// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/padding.h"

#include "ofp/byteorder.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(padding, PadLength) {
  EXPECT_EQ(0, PadLength(0));
  EXPECT_EQ(8, PadLength(8));
  for (unsigned i = 1; i < 8; ++i)
    EXPECT_EQ(8, PadLength(i));

  for (unsigned i = 0; i < 100; ++i) {
    EXPECT_EQ(0, PadLength(i) % 8);
    EXPECT_LE(i, PadLength(i));
    EXPECT_GE(7, PadLength(i) - i);
  }
}

TEST(padding, Padding) {
  {
    Padding<1> pad;
    EXPECT_EQ(1, sizeof(pad));
  }

  {
    Padding<2> pad;
    EXPECT_EQ(2, sizeof(pad));
  }

  {
    Padding<7> pad;
    EXPECT_EQ(7, sizeof(pad));
  }

  {
    Padding<8> pad;
    EXPECT_EQ(8, sizeof(pad));
  }

  {
    Padding<9> pad;
    EXPECT_EQ(9, sizeof(pad));
  }
}

TEST(padding, Padded_Big8) {
  Padded<Big8> b{1};

  EXPECT_EQ(8, sizeof(b));
  EXPECT_HEX("01 000000 00000000", &b, 8);

  EXPECT_EQ(1, b.content());

  b = 0xF1;
  EXPECT_EQ(0xF1, b.content());
  EXPECT_HEX("F1 000000 00000000", &b, 8);
}

TEST(padding, Padded_Big16) {
  Padded<Big16> b{2};

  EXPECT_EQ(8, sizeof(b));
  EXPECT_HEX("0002 000000000000", &b, 8);

  b = 0xF1F2;
  EXPECT_EQ(0xF1F2, b.content());
  EXPECT_HEX("F1F2 000000000000", &b, 8);
}

TEST(padding, Padded_Big32) {
  Padded<Big32> b{3};

  EXPECT_EQ(8, sizeof(b));
  EXPECT_HEX("00000003 00000000", &b, 8);

  b = 0xF1F2F3F4;
  EXPECT_EQ(0xF1F2F3F4, b.content());
  EXPECT_HEX("F1F2F3F4 00000000", &b, 8);
}

TEST(padding, Padded_Big64) {
  Padded<Big64> b{4};

  EXPECT_EQ(8, sizeof(b));
  EXPECT_HEX("0000000000000004", &b, 8);

  b = 0xF1F2F3F4F5F6F7F8ULL;
  EXPECT_EQ(0xF1F2F3F4F5F6F7F8ULL, b.content());
  EXPECT_HEX("F1F2F3F4F5F6F7F8", &b, 8);
}
