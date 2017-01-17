// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmregister.h"
#include "ofp/oxmfields.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(oxmregister, test) {
  OXMRegister reg{OFB_ETH_DST::type(), 24, 24};

  EXPECT_EQ(24, reg.offset());
  EXPECT_EQ(48, reg.end());
  EXPECT_EQ(24, reg.nbits());
  EXPECT_EQ(OFB_ETH_DST::type(), reg.type());
  EXPECT_EQ("ETH_DST[24:48]", reg.toString());
}

TEST(oxmregister, parseValid) {
  OXMRegister reg;

  EXPECT_TRUE(reg.parse("ETH_DST[24:48]"));
  EXPECT_EQ(24, reg.offset());
  EXPECT_EQ(24, reg.nbits());
  EXPECT_EQ("ETH_DST[24:48]", reg.toString());

  EXPECT_TRUE(reg.parse("ETH_TYPE[0:16]"));
  EXPECT_EQ(0, reg.offset());
  EXPECT_EQ(16, reg.nbits());
  EXPECT_EQ("ETH_TYPE[0:16]", reg.toString());

  EXPECT_TRUE(reg.parse("ETH_SRC[36:]"));
  EXPECT_TRUE(reg.parse("ETH_SRC[:36]"));
}

TEST(oxmregister, parseInvalid) {
  OXMRegister reg;

  // Bad range
  EXPECT_FALSE(reg.parse("ETH_TYPE[0:24]"));
  EXPECT_FALSE(reg.parse("ETH_SRC[36:49]"));
  EXPECT_FALSE(reg.parse("ETH_TYPE[16:15]"));
  EXPECT_FALSE(reg.parse("ETH_TYPE[-4:16]"));

  // Bad format
  EXPECT_FALSE(reg.parse("ETH_TYPE[0:16"));
  EXPECT_FALSE(reg.parse("ETH_SRC 36:48]"));
  EXPECT_FALSE(reg.parse("ETH_SRC[36]"));

  // Bad field
  EXPECT_FALSE(reg.parse("ETH_TYP[0:4]"));
  EXPECT_FALSE(reg.parse("ETH_SRC[36]"));
}
