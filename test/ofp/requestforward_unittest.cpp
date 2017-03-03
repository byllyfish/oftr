// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/requestforward.h"
#include "ofp/actionlist.h"
#include "ofp/groupmod.h"
#include "ofp/unittest.h"

using namespace ofp;

const OFPGroupModCommand kFakeCommand = static_cast<OFPGroupModCommand>(0x1111);
const OFPGroupType kFakeType = static_cast<OFPGroupType>(0x22);

TEST(requestforward, test) {
  GroupModBuilder groupMod;
  groupMod.setCommand(kFakeCommand);
  groupMod.setGroupType(kFakeType);
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
  message.normalize();

  const RequestForward *m = RequestForward::cast(&message);
  EXPECT_TRUE(m);

  ByteRange req = m->request();
  EXPECT_EQ(48, req.size());
  EXPECT_HEX(
      "050F00300000000111112200333333330020444455555555666666660000000000000010"
      "01010101AAAA000000000000",
      req.data(), req.size());

  Message request{req.data(), req.size()};
  request.normalize();

  const GroupMod *gm = GroupMod::cast(&request);
  EXPECT_TRUE(gm);

  EXPECT_EQ(0x1111, gm->command());
  EXPECT_EQ(0x22, gm->groupType());
  EXPECT_EQ(0x33333333, gm->groupId());
}

TEST(requestforward, invalid_message) {
  ByteList data{HexToRawData("0620001000000000060F000800000000")};

  Message message{data.data(), data.size()};
  message.normalize();

  const RequestForward *m = RequestForward::cast(&message);
  ASSERT_TRUE(m);

  // RequestForward does *not* validate the inner message contents, just its
  // length.

  ByteRange req = m->request();
  EXPECT_HEX("060F000800000000", req.data(), req.size());
}
