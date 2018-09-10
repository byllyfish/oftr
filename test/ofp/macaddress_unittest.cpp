// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/macaddress.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(macaddress, valid) {
  MacAddress enet{"01:02:03:04:05:06"};

  EXPECT_TRUE(enet.valid());
  EXPECT_EQ("01:02:03:04:05:06", enet.toString());

  MacAddress enet2{"01:02:03:04:05:06:07"};

  EXPECT_FALSE(enet2.valid());
  EXPECT_EQ("00:00:00:00:00:00", enet2.toString());
}

TEST(macaddress, invalid) {
  MacAddress enet;

  EXPECT_FALSE(enet.valid());
  EXPECT_EQ("00:00:00:00:00:00", enet.toString());

  EXPECT_FALSE(enet.parse(""));
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

TEST(macaddress, integer) {
  MacAddress a;

  EXPECT_FALSE(a.parse("0x01"));
  EXPECT_FALSE(a.parse("01"));
  EXPECT_FALSE(a.parse("1"));

  EXPECT_TRUE(a.parse("123456781234"));
  EXPECT_EQ("12:34:56:78:12:34", a.toString());

  EXPECT_TRUE(a.parse("0000ffaabbcc"));
  EXPECT_EQ("00:00:ff:aa:bb:cc", a.toString());

  EXPECT_FALSE(a.parse("0x0000ffaabbcc"));
  // EXPECT_EQ("00:00:ff:aa:bb:cc", a.toString());

  EXPECT_FALSE(a.parse("0000ffaabbc"));
  EXPECT_FALSE(a.parse("0000ffaabbccd"));
  EXPECT_FALSE(a.parse(" 12345678123"));
  EXPECT_FALSE(a.parse("12345678123 "));
  EXPECT_FALSE(a.parse("0x1234567809"));
}

TEST(macaddress, single_hex) {
  MacAddress a;

  EXPECT_TRUE(a.parse("0e:00:00:00:0:73"));
  EXPECT_EQ("0e:00:00:00:00:73", a.toString());
  EXPECT_TRUE(a.parse("e:00:00:00:00:01"));
  EXPECT_EQ("0e:00:00:00:00:01", a.toString());
  EXPECT_TRUE(a.parse("0e:00:00:00:00:1"));
  EXPECT_EQ("0e:00:00:00:00:01", a.toString());
  EXPECT_TRUE(a.parse("1:2:3:4:5:6"));
  EXPECT_EQ("01:02:03:04:05:06", a.toString());

  EXPECT_FALSE(a.parse("1:2:3:4:5:"));
  EXPECT_FALSE(a.parse("1:2:3:4:5"));

  // Delimited; exactly 12 bytes long.
  EXPECT_TRUE(a.parse("00:1:1:1:1:1"));
  EXPECT_EQ("00:01:01:01:01:01", a.toString());
}

TEST(macaddress, dash_delimiter) {
  MacAddress a;

  EXPECT_TRUE(a.parse("01-02-03-04-05-06"));
  EXPECT_EQ("01:02:03:04:05:06", a.toString());
  EXPECT_TRUE(a.parse("01-02-03-04-05-6"));
  EXPECT_EQ("01:02:03:04:05:06", a.toString());
  EXPECT_TRUE(a.parse("1-02-03-04-05-06"));
  EXPECT_EQ("01:02:03:04:05:06", a.toString());
  EXPECT_TRUE(a.parse("1-2-3-4-5-6"));
  EXPECT_EQ("01:02:03:04:05:06", a.toString());

  EXPECT_FALSE(a.parse("1-2-3-4-5"));
  EXPECT_FALSE(a.parse("1-2-3-4-5-6-7"));
}
