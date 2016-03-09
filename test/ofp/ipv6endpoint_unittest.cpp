// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/ipv6endpoint.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(ipv6endpoint, testv4) {
  IPv6Endpoint endpt{"192.168.1.1", 80};

  EXPECT_EQ(80, endpt.port());
  EXPECT_EQ("192.168.1.1", endpt.address().toString());
  EXPECT_EQ("192.168.1.1:80", endpt.toString());
}

TEST(ipv6endpoint, testv6) {
  IPv6Endpoint endpt{"::1", 80};

  EXPECT_EQ(80, endpt.port());
  EXPECT_EQ("::1", endpt.address().toString());
  EXPECT_EQ("[::1]:80", endpt.toString());
}

TEST(ipv6endpoint, parse) {
  IPv6Endpoint endpt;

  EXPECT_TRUE(endpt.parse(" [192.168.1.1] : 80"));
  EXPECT_EQ(IPv6Address{"192.168.1.1"}, endpt.address());
  EXPECT_EQ(80, endpt.port());

  EXPECT_TRUE(endpt.parse("[::1]:79"));
  EXPECT_EQ(IPv6Address{"::1"}, endpt.address());
  EXPECT_EQ(79, endpt.port());

  EXPECT_TRUE(endpt.parse("192.168.1.1:77"));
  EXPECT_EQ(IPv6Address{"192.168.1.1"}, endpt.address());
  EXPECT_EQ(77, endpt.port());

  EXPECT_TRUE(endpt.parse(".820"));
  EXPECT_EQ(IPv6Address{}, endpt.address());
  EXPECT_EQ(820, endpt.port());

  EXPECT_TRUE(endpt.parse(":81"));
  EXPECT_EQ(IPv6Address{}, endpt.address());
  EXPECT_EQ(81, endpt.port());
  EXPECT_FALSE(endpt.parse("xyz"));
  EXPECT_EQ("[::]:81", endpt.toString());  // previous value unaffected
}

TEST(ipv6endpoint, parse2) {
  IPv6Endpoint endpt;

  EXPECT_TRUE(endpt.parse("80"));
  EXPECT_EQ(IPv6Address{}, endpt.address());
  EXPECT_EQ(80, endpt.port());

  EXPECT_TRUE(endpt.parse(":65535"));
  EXPECT_EQ(IPv6Address{}, endpt.address());
  EXPECT_EQ(65535, endpt.port());

  EXPECT_TRUE(endpt.parse("127.000.000.001:06633"));
  EXPECT_EQ(IPv6Address{"127.0.0.1"}, endpt.address());
  EXPECT_EQ(6633, endpt.port());

  EXPECT_TRUE(endpt.parse("127.000.000.001.06633"));
  EXPECT_EQ(IPv6Address{"127.0.0.1"}, endpt.address());
  EXPECT_EQ(6633, endpt.port());

  EXPECT_TRUE(endpt.parse("::1.06633"));
  EXPECT_EQ(IPv6Address{"::1"}, endpt.address());
  EXPECT_EQ(6633, endpt.port());

  EXPECT_TRUE(endpt.parse("::1:06633"));
  EXPECT_EQ(IPv6Address{"::1"}, endpt.address());
  EXPECT_EQ(6633, endpt.port());

  EXPECT_TRUE(endpt.parse("[]:1234"));
  EXPECT_EQ(IPv6Address{}, endpt.address());
  EXPECT_EQ(1234, endpt.port());

  EXPECT_TRUE(endpt.parse(" fe80::1 .4321 "));
  EXPECT_EQ(IPv6Address{"fe80::1"}, endpt.address());
  EXPECT_EQ(4321, endpt.port());
}

TEST(ipv6endpoint, parseFails) {
  IPv6Endpoint endpt;

  EXPECT_FALSE(endpt.parse("1.2.3.4 80"));

  EXPECT_FALSE(endpt.parse("192.168.1.1"));
  EXPECT_FALSE(endpt.parse("2000::1"));
  EXPECT_FALSE(endpt.parse("127.0.0.1"));

  EXPECT_FALSE(endpt.parse("1.2.3.4:0"));
  EXPECT_FALSE(endpt.parse("1.2.3.4:65536"));
}

TEST(ipv6endpoint, relational) {
  IPv6Endpoint a{"[2001::01]:1"};
  IPv6Endpoint b{"[2001::01]:2"};

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

TEST(ipv6endpoint, hash) {}
