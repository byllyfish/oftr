// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/actiontype.h"

using namespace ofp;

TEST(actiontype, AT_OUTPUT) {
  ActionType type{OFPAT_OUTPUT, 8};

  EXPECT_EQ(4, sizeof(type));
  EXPECT_EQ(OFPAT_OUTPUT, type.enumType());
  EXPECT_EQ(8, type.length());
  EXPECT_EQ(0x08, type.nativeType());
  EXPECT_HEX("00000008", &type, sizeof(type));

  EXPECT_EQ(0, type.zeroLength().nativeType());
}

TEST(actiontype, AT_EXPERIMENTER) {
  ActionType type{OFPAT_EXPERIMENTER, 64};

  EXPECT_EQ(4, sizeof(type));
  EXPECT_EQ(OFPAT_EXPERIMENTER, type.enumType());
  EXPECT_EQ(64, type.length());
  EXPECT_EQ(0xFFFF0040, type.nativeType());
  EXPECT_HEX("FFFF0040", &type, sizeof(type));

  EXPECT_EQ(0xFFFF0000, type.zeroLength().nativeType());
}

TEST(actiontype, AT_EXPERIMENTER_2) {
  ActionType type{OFPAT_EXPERIMENTER, 65530};

  EXPECT_EQ(4, sizeof(type));
  EXPECT_EQ(OFPAT_EXPERIMENTER, type.enumType());
  EXPECT_EQ(65530, type.length());
  EXPECT_EQ(0xFFFFFFFA, type.nativeType());

  auto expected = HexToRawData("FFFFFFFA");
  EXPECT_EQ(0, std::memcmp(expected.data(), &type, sizeof(type)));
}
