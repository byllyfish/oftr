#include "ofp/unittest.h"
#include "ofp/rolestatus.h"

using namespace ofp;

TEST(rolestatus, builder) {
    RoleStatusBuilder msg;

    msg.setRole(OFPCR_ROLE_MASTER);
    msg.setReason(0x11);
    msg.setGenerationId(0x2222222222222222);

    MemoryChannel channel{OFP_VERSION_5};
    UInt32 xid = msg.send(&channel);

    EXPECT_EQ(24, channel.size());
    EXPECT_HEX("051E00180000000100000002110000002222222222222222", channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();

  const RoleStatus *m = RoleStatus::cast(&message);
  EXPECT_TRUE(m);

  EXPECT_EQ(OFPCR_ROLE_MASTER, m->role());
  EXPECT_EQ(0x11, m->reason());
  EXPECT_EQ(0x2222222222222222, m->generationId());
}


