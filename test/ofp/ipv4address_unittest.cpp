// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/ipv4address.h"

using namespace ofp;

TEST(ipv4address, test) {
  IPv4Address addr{"192.168.1.1"};

  EXPECT_TRUE(addr.valid());
  EXPECT_EQ("192.168.1.1", addr.toString());
}

TEST(ipv4address, paddedTest) {
  IPv4Address addr{"127.000.090.009"};

  EXPECT_TRUE(addr.valid());
  EXPECT_EQ("127.0.90.9", addr.toString());

  EXPECT_TRUE(addr.parse("001.002.003.0004"));
  EXPECT_EQ("1.2.3.4", addr.toString());

  EXPECT_TRUE(addr.parse("00.00002.003.000"));
  EXPECT_EQ("0.2.3.0", addr.toString());

  EXPECT_TRUE(addr.parse("000000.00000.0000.000"));
  EXPECT_EQ("0.0.0.0", addr.toString());
  EXPECT_FALSE(addr.valid());
}

TEST(ipv4address, mask) {
  IPv4Address a = IPv4Address::mask(0);
  EXPECT_EQ("0.0.0.0", a.toString());

  IPv4Address b = IPv4Address::mask(1);
  EXPECT_EQ("128.0.0.0", b.toString());

  IPv4Address c = IPv4Address::mask(7);
  EXPECT_EQ("254.0.0.0", c.toString());

  IPv4Address d = IPv4Address::mask(8);
  EXPECT_EQ("255.0.0.0", d.toString());

  IPv4Address e = IPv4Address::mask(9);
  EXPECT_EQ("255.128.0.0", e.toString());

  IPv4Address f = IPv4Address::mask(23);
  EXPECT_EQ("255.255.254.0", f.toString());

  IPv4Address g = IPv4Address::mask(24);
  EXPECT_EQ("255.255.255.0", g.toString());

  IPv4Address h = IPv4Address::mask(25);
  EXPECT_EQ("255.255.255.128", h.toString());

  IPv4Address i = IPv4Address::mask(31);
  EXPECT_EQ("255.255.255.254", i.toString());

  IPv4Address j = IPv4Address::mask(32);
  EXPECT_EQ("255.255.255.255", j.toString());

  EXPECT_FALSE(i.isBroadcast());
  EXPECT_TRUE(j.isBroadcast());
}

TEST(ipv4address, prefix) {
  for (unsigned i = 0; i < 33; ++i) {
    IPv4Address addr = IPv4Address::mask(i);
    EXPECT_EQ(i, addr.prefix());
  }
}

TEST(ipv4address, valid) {
  IPv4Address addr;
  EXPECT_FALSE(addr.valid());

  std::array<UInt8, 4> buf = {{1, 2, 3, 4}};

  IPv4Address *x = new (&buf) IPv4Address;
  EXPECT_FALSE(x->valid());
}

TEST(ipv4address, misaligned) {
  // IPv4Address should be "packed" when placed in a struct.

  struct TestBuf {
    UInt8 ignore;
    IPv4Address addr;
  };

  static_assert(sizeof(TestBuf) == 5, "Unexpected size");
  static_assert(offsetof(TestBuf, addr) == 1, "Unexpected offset");

  TestBuf buf;
  EXPECT_FALSE(buf.addr.valid());
  EXPECT_TRUE(buf.addr.parse("251.252.253.254"));
  EXPECT_TRUE(buf.addr.valid());
  EXPECT_EQ("251.252.253.254", buf.addr.toString());
}

TEST(ipv4address, invalid) {
  IPv4Address addr;

  EXPECT_FALSE(addr.parse(""));
  EXPECT_FALSE(addr.parse(" 1.2.3.4 "));
  EXPECT_FALSE(addr.parse("1"));
  EXPECT_FALSE(addr.parse("1.2"));
  EXPECT_FALSE(addr.parse("1.2."));
  EXPECT_FALSE(addr.parse("1.2.3"));
  EXPECT_FALSE(addr.parse("1.2.3."));
  EXPECT_FALSE(addr.parse("1.2.3.4."));
  EXPECT_FALSE(addr.parse(".1.2.3"));
  EXPECT_FALSE(addr.parse("1.2.3.4.5"));
  EXPECT_FALSE(addr.parse("1.2..4"));
  EXPECT_FALSE(addr.parse("a.b.c.d"));
  EXPECT_FALSE(addr.parse("1.2.3.256"));
  EXPECT_FALSE(addr.parse("256.2.3.4"));
  EXPECT_FALSE(addr.parse("-1.2.3.4"));
}
