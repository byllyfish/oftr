// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/macaddress.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(macaddress, valid) {
  MacAddress enet{"01:02:03:04:05:06"};

  EXPECT_TRUE(enet.valid());
  EXPECT_EQ("01:02:03:04:05:06", enet.toString());

  MacAddress enet2{"01-02-03-04-05-06-07"};  // okay

  EXPECT_TRUE(enet2.valid());
  EXPECT_EQ("01:02:03:04:05:06", enet2.toString());
}

TEST(macaddress, invalid) {
  MacAddress enet;

  EXPECT_FALSE(enet.valid());
  EXPECT_EQ("00:00:00:00:00:00", enet.toString());
}

TEST(macaddress, invalid2) {
  MacAddress enet{"01:02:03:04:05"};

  EXPECT_FALSE(enet.valid());
  EXPECT_EQ("00:00:00:00:00:00", enet.toString());
}

TEST(macaddress, hash) {
  MacAddress enet1{"01:02:03:04:05:06"};

  std::hash<MacAddress> hasher;
  EXPECT_EQ(0xf6fb78f6, hasher(enet1));

  MacAddress enet2{"01:02:03:04:05:07"};

  EXPECT_EQ(0xe5dfcd2d, hasher(enet2));
}

TEST(macaddress, canonical) {
  MacAddress enet{"aa:bb:cc:dd:ee:ff"};

  EXPECT_TRUE(enet.valid());
  EXPECT_EQ("aa:bb:cc:dd:ee:ff", enet.toString());

  MacAddress enet2{"AA:BB:CC:DD:EE:FF"};  // okay

  EXPECT_TRUE(enet2.valid());
  EXPECT_EQ("aa:bb:cc:dd:ee:ff", enet2.toString());
}

TEST(macaddress, relational) {
  MacAddress a{"01:00:00:00:00:01"};
  MacAddress b{"01:00:00:00:00:02"};

  EXPECT_TRUE(a < b);
  EXPECT_FALSE(a > b);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(a >= b);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);

  EXPECT_FALSE(b < a);
  EXPECT_FALSE(b <= a);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(b >= a);
  EXPECT_FALSE(b == a);
  EXPECT_TRUE(b != a);
}
