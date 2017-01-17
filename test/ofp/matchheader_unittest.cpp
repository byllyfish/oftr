// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchheader.h"
#include "ofp/unittest.h"
#include "ofp/validation.h"

using namespace ofp;

static bool isMatchHeaderValid(const char *hex) {
  Validation context;
  ByteList s{HexToRawData(hex)};
  const MatchHeader *hdr = reinterpret_cast<const MatchHeader *>(s.data());
  return hdr->validateInput(s.size(), &context);
}

TEST(matchheader, test) {
  EXPECT_FALSE(isMatchHeaderValid(""));
  EXPECT_FALSE(isMatchHeaderValid("00"));
  EXPECT_FALSE(isMatchHeaderValid("0001"));
  EXPECT_FALSE(isMatchHeaderValid("00010003"));
  EXPECT_FALSE(isMatchHeaderValid("00010001"));
  EXPECT_FALSE(isMatchHeaderValid("00010000"));
  EXPECT_FALSE(isMatchHeaderValid("00010004"));
  EXPECT_FALSE(isMatchHeaderValid("00010004 000000"));
  EXPECT_FALSE(isMatchHeaderValid("00010004 0000"));
  EXPECT_FALSE(isMatchHeaderValid("00010004 00"));

  EXPECT_TRUE(isMatchHeaderValid("00010004 00000000"));
  EXPECT_TRUE(isMatchHeaderValid("00010008 80000000"));

  // Padding doesn't have to be zero.
  EXPECT_TRUE(isMatchHeaderValid("00010004 FF000000"));

  EXPECT_TRUE(isMatchHeaderValid(
      "0001003480000408FFFFFFFFFFFFFFFF80000408FFFFFFFFFFFFFFFF80000203FFFFFFFF"
      "80000D04FFFFFFFF80000D04FFFFFFFF00000000"));
}
