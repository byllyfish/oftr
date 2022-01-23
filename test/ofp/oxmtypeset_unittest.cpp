// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmtypeset.h"

#include "ofp/unittest.h"

using namespace ofp;

TEST(oxmtypeset, set_and_find) {
  OXMTypeSet set;

  EXPECT_TRUE(set.add(OXMType(0x8000, 1, 16)));
  EXPECT_TRUE(set.add(OXMType(0x8000, 2, 32)));
  EXPECT_FALSE(set.add(OXMType(0x8000, 1, 16)));
  EXPECT_TRUE(set.find(OXMType(0x8000, 1, 64)));

  EXPECT_TRUE(set.find(OXMType(0x8000, 1, 16)));
  EXPECT_TRUE(set.find(OXMType(0x8000, 1, 64)));
  EXPECT_FALSE(set.find(OXMType(0x8000, 3, 32)));
  EXPECT_FALSE(set.find(OXMType(0x9000, 1, 32)));

  EXPECT_TRUE(set.add(OXMType(0x9000, 1, 16)));
  EXPECT_TRUE(set.find(OXMType(0x9000, 1, 16)));
}

TEST(oxmtypeset, limits) {
  OXMTypeSet set;

  EXPECT_TRUE(set.add(OXMType(0x8000, 0x7F, 16)));
  EXPECT_TRUE(set.add(OXMType(0x8000, 0x7E, 32)));
  EXPECT_FALSE(set.add(OXMType(0x8000, 0x7F, 16)));
  EXPECT_TRUE(set.find(OXMType(0x8000, 0x7E, 64)));

  EXPECT_FALSE(set.find(OXMType(0x8000, 0x80, 16)));
  EXPECT_TRUE(set.find(OXMType(0x8000, 0xFF, 64)));
  EXPECT_FALSE(set.find(OXMType(0x8000, 0xF9, 32)));
  EXPECT_TRUE(set.find(OXMType(0x8000, 0x7F, 128)));
  EXPECT_FALSE(set.find(OXMType(0x9000, 0x7F, 32)));

  EXPECT_TRUE(set.add(OXMType(0x9000, 0x7F, 16)));
  EXPECT_TRUE(set.find(OXMType(0x9000, 0x7F, 16)));
}
