// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/actiontype.h"

#include "ofp/unittest.h"

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

TEST(actiontype, parse) {
  ActionType action;

  EXPECT_TRUE(action.parse("COPY_TTL_OUT"));
  EXPECT_EQ(0x000B0008, action.nativeType());

  EXPECT_TRUE(action.parse("ENQUEUE"));
  EXPECT_EQ(0x000B0010, action.nativeType());
}

TEST(actiontype, lookupInfo) {
  ActionType action1{OFPAT_OUTPUT, 16};
  EXPECT_STREQ("OUTPUT", action1.lookupInfo()->name);

  ActionType action2{OFPAT_OUTPUT, 24};
  EXPECT_EQ(nullptr, action2.lookupInfo());

  ActionType action3{OFPAT_EXPERIMENTER, 24};
  EXPECT_STREQ("EXPERIMENTER", action3.lookupInfo()->name);

  ActionType action4{static_cast<OFPActionType>(deprecated::v1::OFPAT_ENQUEUE),
                     16};
  EXPECT_STREQ("ENQUEUE", action4.lookupInfo()->name);

  ActionType action5{OFPAT_COPY_TTL_OUT, 8};
  EXPECT_STREQ("COPY_TTL_OUT", action5.lookupInfo()->name);
}
