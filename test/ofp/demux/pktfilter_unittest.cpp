// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/pktfilter.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(pktfilter, ether_src) {
  demux::PktFilter filter;

  ASSERT_TRUE(filter.setFilter("ether src 00:00:00:00:00:01"));
  EXPECT_EQ(filter.filter(), "ether src 00:00:00:00:00:01");

  {  // Test packet with eth_src that matches.
    ByteList data{HexToRawData("000000000002 000000000001 0800")};
    EXPECT_TRUE(filter.match(data.toRange()));
  }

  {  // Test packet with eth_src that doesn't match.
    ByteList data{HexToRawData("000000000001 000000000002 0800")};
    EXPECT_FALSE(filter.match(data.toRange()));
  }
}

TEST(pktfilter, empty) {
  demux::PktFilter filter;
  EXPECT_TRUE(filter.empty());
  EXPECT_EQ(filter.filter(), "");

  ByteList data{HexToRawData("000000000002 000000000001 0800")};
  EXPECT_FALSE(filter.match(data.toRange()));

  filter.clear();
  EXPECT_TRUE(filter.empty());
  EXPECT_TRUE(filter.setFilter("ip"));
  EXPECT_FALSE(filter.empty());
  EXPECT_EQ(filter.filter(), "ip");
  filter.clear();
  EXPECT_TRUE(filter.empty());
  EXPECT_EQ(filter.filter(), "");
}

TEST(pktfilter, move) {
  demux::PktFilter filter;
  EXPECT_TRUE(filter.empty());
  EXPECT_TRUE(filter.setFilter("ip"));
  EXPECT_FALSE(filter.empty());

  demux::PktFilter filter2{std::move(filter)};
  // "filter" has been moved-from.
  EXPECT_TRUE(filter.empty());
  EXPECT_EQ(filter.filter(), "");
  EXPECT_FALSE(filter2.empty());
  EXPECT_EQ(filter2.filter(), "ip");
}

TEST(pktfilter, setFilter) {
  demux::PktFilter filter;
  EXPECT_TRUE(filter.setFilter("ip"));
  EXPECT_EQ(filter.filter(), "ip");
  EXPECT_FALSE(filter.empty());

  // When we replace a filter, it should remain unchanged if there is an error.
  EXPECT_FALSE(filter.setFilter("moo moo moo"));
  EXPECT_EQ(filter.filter(), "ip");

  // Make sure we can replace existing filter without a memory leak.
  EXPECT_TRUE(filter.setFilter("icmp"));
  EXPECT_EQ(filter.filter(), "icmp");
}
