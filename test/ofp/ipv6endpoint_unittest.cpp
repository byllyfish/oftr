// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/ipv6endpoint.h"

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

  EXPECT_TRUE(endpt.parse(":81"));
  EXPECT_EQ(IPv6Address{}, endpt.address());
  EXPECT_EQ(81, endpt.port());

  EXPECT_TRUE(endpt.parse("82"));
  EXPECT_EQ(IPv6Address{}, endpt.address());
  EXPECT_EQ(82, endpt.port());

  EXPECT_FALSE(endpt.parse("192.168.1.1"));
  EXPECT_FALSE(endpt.parse("2000::1"));
  EXPECT_FALSE(endpt.parse("127.0.0.1"));

  EXPECT_EQ("[::]:82", endpt.toString());
}
