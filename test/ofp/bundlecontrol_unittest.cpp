// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/bundlecontrol.h"
#include "ofp/bundleproperty.h"

using namespace ofp;

TEST(bundlecontrol, test) {
  BundleControlBuilder msg;
  msg.setBundleId(0x22222222);
  msg.setCtrlType(0x3333);
  msg.setFlags(0x4444);

  PropertyList props;
  props.add(BundlePropertyExperimenter{0x12345678, 0xABACABAC, {"foo", 3}});
  msg.setProperties(props);

  MemoryChannel channel{OFP_VERSION_5};
  (void)msg.send(&channel);

  EXPECT_EQ(32, channel.size());
  EXPECT_HEX("05210020000000012222222233334444FFFF000F12345678ABACABAC666F6F00",
             channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();

  const BundleControl *m = BundleControl::cast(&message);
  EXPECT_TRUE(m);

  EXPECT_EQ(0x22222222, m->bundleId());
  EXPECT_EQ(0x3333, m->ctrlType());
  EXPECT_EQ(0x4444, m->flags());

  EXPECT_EQ(1, m->properties().itemCount());

  for (auto &iter : m->properties()) {
    EXPECT_EQ(BundlePropertyExperimenter::type(), iter.type());
    auto &expProp = iter.property<BundlePropertyExperimenter>();
    EXPECT_EQ(0x12345678, expProp.experimenter());
    EXPECT_EQ(0xABACABAC, expProp.expType());
    EXPECT_EQ(ByteRange("foo", 3), expProp.expData());
  }
}
