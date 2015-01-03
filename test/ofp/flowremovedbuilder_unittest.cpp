// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/flowremoved.h"

using namespace ofp;

TEST(flowremovedbuilder, v1_3) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  FlowRemovedBuilder flowRemoved;

  flowRemoved.setCookie(0x1111111111111111);
  flowRemoved.setPriority(0x2222);
  flowRemoved.setReason(static_cast<OFPFlowRemovedReason>(0x33));
  flowRemoved.setTableId(0x44);
  flowRemoved.setDurationSec(0x55555555);
  flowRemoved.setDurationNSec(0x66666666);
  flowRemoved.setIdleTimeout(0x7777);
  flowRemoved.setHardTimeout(0x8888);
  flowRemoved.setPacketCount(0x9999999999999999);
  flowRemoved.setByteCount(0xAAAAAAAAAAAAAAAA);
  flowRemoved.setMatch(match);

  MemoryChannel channel{0x04};
  UInt32 xid = flowRemoved.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x040, channel.size());

  const char *expected =
      "040B0040000000011111111111111111222233445555555566666"
      "666777788889999999999999999AAAAAAAAAAAAAAAA0001000C80"
      "0000040000001B00000000";
  EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowremovedbuilder, v1_0) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  FlowRemovedBuilder flowRemoved;

  flowRemoved.setCookie(0x1111111111111111);
  flowRemoved.setPriority(0x2222);
  flowRemoved.setReason(static_cast<OFPFlowRemovedReason>(0x33));
  flowRemoved.setTableId(0x44);
  flowRemoved.setDurationSec(0x55555555);
  flowRemoved.setDurationNSec(0x66666666);
  flowRemoved.setIdleTimeout(0x7777);
  flowRemoved.setHardTimeout(0x8888);
  flowRemoved.setPacketCount(0x9999999999999999);
  flowRemoved.setByteCount(0xAAAAAAAAAAAAAAAA);
  flowRemoved.setMatch(match);

  MemoryChannel channel{0x01};
  UInt32 xid = flowRemoved.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(88, channel.size());

  const char *expected =
      "010B005800000001003820FE001B0000000000000000000000000"
      "00000000000000000000000000000000000000000001111111111"
      "11111122223300555555556666666677770000999999999999999"
      "9AAAAAAAAAAAAAAAA";
  EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowremovedbuilder, v1_1) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  FlowRemovedBuilder flowRemoved;

  flowRemoved.setCookie(0x1111111111111111);
  flowRemoved.setPriority(0x2222);
  flowRemoved.setReason(static_cast<OFPFlowRemovedReason>(0x33));
  flowRemoved.setTableId(0x44);
  flowRemoved.setDurationSec(0x55555555);
  flowRemoved.setDurationNSec(0x66666666);
  flowRemoved.setIdleTimeout(0x7777);
  flowRemoved.setHardTimeout(0x8888);
  flowRemoved.setPacketCount(0x9999999999999999);
  flowRemoved.setByteCount(0xAAAAAAAAAAAAAAAA);
  flowRemoved.setMatch(match);

  MemoryChannel channel{0x02};
  UInt32 xid = flowRemoved.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(136, channel.size());

  const char *expected =
      "020B00880000000111111111111111112222334455555555666666667777000099999999"
      "99999999AAAAAAAAAAAAAAAA000000580000001B000003FE000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000";
  EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowremovedbuilder, v1_2) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  FlowRemovedBuilder flowRemoved;

  flowRemoved.setCookie(0x1111111111111111);
  flowRemoved.setPriority(0x2222);
  flowRemoved.setReason(static_cast<OFPFlowRemovedReason>(0x33));
  flowRemoved.setTableId(0x44);
  flowRemoved.setDurationSec(0x55555555);
  flowRemoved.setDurationNSec(0x66666666);
  flowRemoved.setIdleTimeout(0x7777);
  flowRemoved.setHardTimeout(0x8888);
  flowRemoved.setPacketCount(0x9999999999999999);
  flowRemoved.setByteCount(0xAAAAAAAAAAAAAAAA);
  flowRemoved.setMatch(match);

  MemoryChannel channel{0x03};
  UInt32 xid = flowRemoved.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x40, channel.size());

  const char *expected =
      "030B0040000000011111111111111111222233445555555566666"
      "666777788889999999999999999AAAAAAAAAAAAAAAA0001000C80"
      "0000040000001B00000000";
  EXPECT_HEX(expected, channel.data(), channel.size());
}
