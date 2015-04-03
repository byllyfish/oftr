// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/requestforward.h"
#include "ofp/groupmod.h"
#include "ofp/actionlist.h"

using namespace ofp;

TEST(requestforward, test) {
  GroupModBuilder groupMod;
  groupMod.setCommand(0x1111);
  groupMod.setGroupType(0x22);
  groupMod.setGroupId(0x33333333);

  ActionList actions;
  actions.add(AT_OUTPUT{0x01010101, 0xAAAA});

  BucketBuilder bkt;
  bkt.setWeight(0x4444);
  bkt.setWatchPort(0x55555555);
  bkt.setWatchGroup(0x66666666);
  bkt.setActions(actions);

  BucketList buckets;
  buckets.add(bkt);
  groupMod.setBuckets(buckets);

  MemoryChannel channel{OFP_VERSION_5};
  groupMod.send(&channel);

  EXPECT_EQ(48, channel.size());
  EXPECT_HEX(
      "050F00300000000111112200333333330020444455555555666666660000000000000010"
      "01010101AAAA000000000000",
      channel.data(), channel.size());

  RequestForwardBuilder reqForw;
  reqForw.setRequest(ByteRange{channel.data(), channel.size()});

  channel.clear();
  reqForw.send(&channel);

  EXPECT_EQ(56, channel.size());
  EXPECT_HEX(
      "0520003800000002050F0030000000011111220033333333002044445555555566666666"
      "000000000000001001010101AAAA000000000000",
      channel.data(), channel.size());

  Message message(channel.data(), channel.size());
  message.transmogrify();

  const RequestForward *m = RequestForward::cast(&message);
  EXPECT_TRUE(m);

  ByteRange req = m->request();
  EXPECT_EQ(48, req.size());
  EXPECT_HEX(
      "050F00300000000111112200333333330020444455555555666666660000000000000010"
      "01010101AAAA000000000000",
      req.data(), req.size());

  Message request{req.data(), req.size()};
  request.transmogrify();

  const GroupMod *gm = GroupMod::cast(&request);
  EXPECT_TRUE(gm);

  EXPECT_EQ(0x1111, gm->command());
  EXPECT_EQ(0x22, gm->groupType());
  EXPECT_EQ(0x33333333, gm->groupId());
}
