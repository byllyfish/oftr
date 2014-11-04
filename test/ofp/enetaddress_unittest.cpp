// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/enetaddress.h"

using namespace ofp;

TEST(enetaddress, valid) {
  EnetAddress enet{"01:02:03:04:05:06"};

  EXPECT_TRUE(enet.valid());
  EXPECT_EQ("01:02:03:04:05:06", enet.toString());

  EnetAddress enet2{"01-02-03-04-05-06-07"}; // okay

  EXPECT_TRUE(enet2.valid());
  EXPECT_EQ("01:02:03:04:05:06", enet2.toString());
}

TEST(enetaddress, invalid) {
  EnetAddress enet{"01:02:03:04:05"};

  EXPECT_FALSE(enet.valid());
  EXPECT_EQ("00:00:00:00:00:00", enet.toString());
}

TEST(enetaddress, hash) {
  EnetAddress enet1{"01:02:03:04:05:06"};

  std::hash<EnetAddress> hasher;
  EXPECT_EQ(176514621, hasher(enet1));

  EnetAddress enet2{"01:02:03:04:05:07"};

  EXPECT_EQ(205143772, hasher(enet2));
}

TEST(enetaddress, canonical) {
  EnetAddress enet{"aa:bb:cc:dd:ee:ff"};

  EXPECT_TRUE(enet.valid());
  EXPECT_EQ("aa:bb:cc:dd:ee:ff", enet.toString());

  EnetAddress enet2{"AA:BB:CC:DD:EE:FF"}; // okay

  EXPECT_TRUE(enet2.valid());
  EXPECT_EQ("aa:bb:cc:dd:ee:ff", enet2.toString());
}
