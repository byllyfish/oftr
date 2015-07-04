// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/rolestatus.h"
#include "ofp/rolestatusproperty.h"

using namespace ofp;

TEST(rolestatus, builder) {
  RoleStatusBuilder msg;

  msg.setRole(OFPCR_ROLE_MASTER);
  msg.setReason(0x11);
  msg.setGenerationId(0x2222222222222222);

  PropertyList props;
  props.add(RoleStatusPropertyExperimenter{0x12345678, 0xABACABAC, {"foo", 3}});
  msg.setProperties(props);

  MemoryChannel channel{OFP_VERSION_5};
  (void)msg.send(&channel);

  EXPECT_EQ(40, channel.size());
  EXPECT_HEX(
      "051E00280000000100000002110000002222222222222222FFFF000F12345678ABACABAC"
      "666F6F00",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();

  const RoleStatus *m = RoleStatus::cast(&message);
  EXPECT_TRUE(m);

  EXPECT_EQ(OFPCR_ROLE_MASTER, m->role());
  EXPECT_EQ(0x11, m->reason());
  EXPECT_EQ(0x2222222222222222, m->generationId());

  EXPECT_EQ(1, m->properties().itemCount());

  for (auto &iter : m->properties()) {
    EXPECT_EQ(RoleStatusPropertyExperimenter::type(), iter.type());
    auto &expProp = iter.property<RoleStatusPropertyExperimenter>();
    EXPECT_EQ(0x12345678, expProp.experimenter());
    EXPECT_EQ(0xABACABAC, expProp.expType());
    EXPECT_EQ(ByteRange("foo", 3), expProp.expData());
  }
}
