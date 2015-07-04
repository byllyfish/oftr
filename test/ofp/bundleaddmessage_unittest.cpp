// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/bundleaddmessage.h"
#include "ofp/groupmod.h"
#include "ofp/actionlist.h"
#include "ofp/bundleproperty.h"
#include "ofp/echorequest.h"

using namespace ofp;

const OFPGroupModCommand kFakeCommand = static_cast<OFPGroupModCommand>(0x1111);
const OFPGroupType kFakeType = static_cast<OFPGroupType>(0x22);

TEST(bundleaddmessage, groupMod) {
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

  BundleAddMessageBuilder msg;
  msg.setBundleId(0x22222222);
  msg.setFlags(0x3333);
  msg.setMessage(ByteRange{channel.data(), channel.size()});

  PropertyList props;
  props.add(BundlePropertyExperimenter{0x12345678, 0xABACABAC, {"foo", 3}});
  msg.setProperties(props);

  channel.clear();
  (void)msg.send(&channel);

  EXPECT_EQ(80, channel.size());
  EXPECT_HEX(
      "05220050000000022222222200003333050F003000000001111122003333333300204444"
      "5555555566666666000000000000001001010101AAAA000000000000FFFF000F12345678"
      "ABACABAC666F6F00",
      channel.data(), channel.size());

  Message message(channel.data(), channel.size());
  message.transmogrify();

  const BundleAddMessage *m = BundleAddMessage::cast(&message);
  EXPECT_TRUE(m);

  EXPECT_EQ(0x22222222, m->bundleId());
  EXPECT_EQ(0x3333, m->flags());

  ByteRange req = m->message();
  EXPECT_EQ(48, req.size());
  EXPECT_HEX(
      "050F00300000000111112200333333330020444455555555666666660000000000000010"
      "01010101AAAA000000000000",
      req.data(), req.size());
}

TEST(bundleaddmessage, echoreq) {
  // Construct echo request that is not padded.
  EchoRequestBuilder echo;
  echo.setEchoData("12345", 5);

  MemoryChannel channel{OFP_VERSION_5};
  echo.send(&channel);

  EXPECT_EQ(13, channel.size());
  EXPECT_HEX("0502000D000000013132333435", channel.data(), channel.size());

  BundleAddMessageBuilder msg;
  msg.setBundleId(0x22222222);
  msg.setFlags(0x3333);
  msg.setMessage(ByteRange{channel.data(), channel.size()});

  PropertyList props;
  props.add(BundlePropertyExperimenter{0x12345678, 0xABACABAC, {"foo", 3}});
  msg.setProperties(props);

  channel.clear();
  (void)msg.send(&channel);

  EXPECT_EQ(48, channel.size());
  EXPECT_HEX(
      "052200300000000222222222000033330502000D000000013132333435000000FFFF000F"
      "12345678ABACABAC666F6F00",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();

  const BundleAddMessage *m = BundleAddMessage::cast(&message);
  EXPECT_TRUE(m);
  EXPECT_EQ(0x22222222, m->bundleId());
  EXPECT_EQ(0x3333, m->flags());

  ByteRange bmsg = m->message();
  EXPECT_EQ(13, bmsg.size());
  EXPECT_HEX("0502000D000000013132333435", bmsg.data(), bmsg.size());

  EXPECT_EQ(1, m->properties().itemCount());

  for (auto &iter : m->properties()) {
    EXPECT_EQ(BundlePropertyExperimenter::type(), iter.type());
    auto &expProp = iter.property<BundlePropertyExperimenter>();
    EXPECT_EQ(0x12345678, expProp.experimenter());
    EXPECT_EQ(0xABACABAC, expProp.expType());
    EXPECT_EQ(ByteRange("foo", 3), expProp.expData());
  }
}
