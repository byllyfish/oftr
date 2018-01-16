// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/ipv6address.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/unittest.h"

using namespace ofp;
using namespace ofp::sys;

TEST(boost_asio, tcp) {
  auto endpt = makeEndpoint<tcp>(IPv6Address{"192.168.1.1"}, 80);

  log_debug("endpt", endpt.address().to_string());

  IPv6Address addr = makeIPv6Address(endpt.address());
  UInt16 port = endpt.port();

  EXPECT_EQ("192.168.1.1", addr.toString());
  EXPECT_EQ(80, port);
}

TEST(boost_asio, udp) {
  auto endpt = makeEndpoint<udp>(IPv6Address{"192.168.1.1"}, 80);

  IPv6Address addr = makeIPv6Address(endpt.address());
  UInt16 port = endpt.port();

  EXPECT_EQ("192.168.1.1", addr.toString());
  EXPECT_EQ(80, port);
}
