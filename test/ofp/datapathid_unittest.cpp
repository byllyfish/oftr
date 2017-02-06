// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/datapathid.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(datapathid, test) {
  DatapathID a;
  EXPECT_HEX("0000 0000 0000 0000", &a, sizeof(a));
  EXPECT_EQ(0, a.implementerDefined());
  EXPECT_EQ("00:00:00:00:00:00:00:00", a.toString());
  EXPECT_TRUE(a.empty());

  DatapathID b{0x1234, MacAddress{"00-01-02-03-04-05"}};
  EXPECT_EQ(0x1234, b.implementerDefined());
  EXPECT_EQ(MacAddress{"00-01-02-03-04-05"}, b.macAddress());
  EXPECT_HEX("1234 00 01 02 03 04 05", &b, sizeof(b));
  EXPECT_EQ("12:34:00:01:02:03:04:05", b.toString());

  EXPECT_LT(a, b);
  EXPECT_NE(a, b);

  DatapathID c{b};
  EXPECT_EQ(b, c);

  DatapathID d{0xffff, MacAddress{"ff:ff:ff:ff:ff:ff"}};
  EXPECT_EQ(0xffff, d.implementerDefined());
  EXPECT_EQ(MacAddress{"ff-ff-ff-ff-ff-ff"}, d.macAddress());
  EXPECT_HEX("ffff ff ff ff ff ff ff", &d, sizeof(d));
  EXPECT_EQ("ff:ff:ff:ff:ff:ff:ff:ff", d.toString());

  // Parsing the empty string should yield an empty datapath.
  DatapathID e;
  EXPECT_TRUE(e.parse(""));
  EXPECT_EQ("00:00:00:00:00:00:00:00", e.toString());
  EXPECT_TRUE(e.empty());

  DatapathID f;
  // There must be enough data to fill 8 bytes...
  EXPECT_FALSE(f.parse("aa:bb:cc:dd:aa:bb:cc:d"));
  EXPECT_TRUE(f.parse("aa:bb:cc:dd:aa:bb:cc:dd"));
  EXPECT_HEX("aa:bb:cc:dd:aa:bb:cc:dd", &f, sizeof(f));
  // It's NOT okay to pass more data than necessary...
  EXPECT_FALSE(f.parse("aa:bb:cc:dd:aa:bb:cc:dd:ee"));
}

TEST(datapathid, relational) {
  DatapathID a{"00:01:02:03:04:00:00:01"};
  DatapathID b{"00:01:02:03:04:00:00:02"};

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

TEST(datapathid, hash) {
  DatapathID a{"00:01:02:03:04:00:00:01"};
  DatapathID b{"00:01:02:03:04:00:00:02"};
  DatapathID c{"00:01:02:03:04:00:00:03"};

  std::hash<DatapathID> hasher;
  EXPECT_EQ(0xe90c4686, hasher(a) & 0xffffffff);
  EXPECT_EQ(0x63511356, hasher(b) & 0xffffffff);
  EXPECT_EQ(0xeff070c0, hasher(c) & 0xffffffff);
}

TEST(datapathid, stream) {
  std::string buf;
  llvm::raw_string_ostream oss{buf};

  DatapathID a{"00:01:02:a3:04:00:00:f1"};
  DatapathID b;

  oss << a << ',' << b << '.';

  EXPECT_EQ("00:01:02:a3:04:00:00:f1,00:00:00:00:00:00:00:00.", oss.str());
}

TEST(datapathid, integer) {
  DatapathID a;

  EXPECT_TRUE(a.parse("0x201"));
  EXPECT_EQ("00:00:00:00:00:00:02:01", a.toString());

  EXPECT_FALSE(a.parse("01"));
  EXPECT_FALSE(a.parse("1"));
  EXPECT_FALSE(a.parse("1234567812345678"));
  EXPECT_FALSE(a.parse("0x1FFFFFFFFFFFFFFFF"));
}
