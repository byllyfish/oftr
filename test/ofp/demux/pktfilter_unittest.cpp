#include "ofp/demux/pktfilter.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(pktfilter, ether_src) {
  demux::PktFilter filter;

  ASSERT_TRUE(filter.setFilter("ether src 00:00:00:00:00:01"));

  {  // Test packet with eth_src that matches.
    ByteList data{HexToRawData("000000000002 000000000001 0800")};
    EXPECT_TRUE(filter.match(data.toRange()));
  }

  {  // Test packet with eth_src that doesn't match.
    ByteList data{HexToRawData("000000000001 000000000002 0800")};
    EXPECT_FALSE(filter.match(data.toRange()));
  }
}
