#include "ofp/unittest.h"
#include "ofp/message.h"
#include "ofp/flowmod.h"

using namespace ofp;

TEST(message, constructor) {
  Message message{nullptr};
  EXPECT_EQ(8, message.size());
}

TEST(message, transmogrify) {
  const char *hexBefore =
      "010E 0048 0000 002C 0010 001F 0000 0000 0000 0000 0000 0000 0000 0000 "
      "0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 "
      "0003 0000 0000 8000 FFFF FFFF FFFF 0000";
  auto s = HexToRawData(hexBefore);

  Message msg{nullptr};
  std::memcpy(msg.mutableData(s.length()), s.data(), s.length());

  EXPECT_EQ(0x48, msg.size());
  EXPECT_HEX(hexBefore, msg.data(), msg.size());

  msg.transmogrify();

  const char *hexAfter =
      "010E00880000002C0000000000000000FFFFFFFFFFFFFFFF0003000000008000FFFFFFFF"
      "FFFFFFFF000000000000000000000058000000000000030F000000000000000000000000"
      "000000000000000000000000000000000000000000000000FFFFFFFF00000000FFFFFFFF"
      "00000000000000000000000000000000000000000000000000000000";

  EXPECT_EQ(0x088, msg.size());
  EXPECT_HEX(hexAfter, msg.data(), msg.size());
}

TEST(message, transmogrify_flow_mod) {
  const char *hexBefore = "010E 0048 0000 0060 0010 001F 0000 0000 0000 0000 "
                          "0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 "
                          "0000 0000 0000 0000 0000 0000 0000 0000 0003 0000 "
                          "0000 8000 FFFF FFFF FFFF 0000";

  auto s = HexToRawData(hexBefore);

  Message message{nullptr};
  std::memcpy(message.mutableData(s.length()), s.data(), s.length());

  message.transmogrify();

  const FlowMod *msg = FlowMod::cast(&message);
  EXPECT_TRUE(msg);

  if (msg) {
    EXPECT_EQ(0, msg->cookie());
    EXPECT_EQ(0xFFFFFFFFFFFFFFFFUL, msg->cookieMask());
    EXPECT_EQ(0, msg->tableId());
    EXPECT_EQ(3, msg->command());
    EXPECT_EQ(0, msg->idleTimeout());
    EXPECT_EQ(0, msg->hardTimeout());
    EXPECT_EQ(0x8000, msg->priority());
    EXPECT_EQ(0xFFFFFFFFU, msg->bufferId());
    EXPECT_EQ(0xFFFFFFFFU, msg->outPort());
    EXPECT_EQ(0, msg->outGroup());
    EXPECT_EQ(0, msg->flags());
  }
}

TEST(message, translateType) {
  for (UInt8 type = 0; type <= OFPT_LAST; ++type) {
    EXPECT_EQ(type, Header::translateType(4, type, 4));
  }

  for (UInt8 type = 0; type <= OFPT_FLOW_MOD; ++type) {
    EXPECT_EQ(type, Header::translateType(1, type, 4));
    EXPECT_EQ(type, Header::translateType(2, type, 4));
    EXPECT_EQ(type, Header::translateType(3, type, 4));
    EXPECT_EQ(type, Header::translateType(4, type, 1));
    EXPECT_EQ(type, Header::translateType(4, type, 2));
    EXPECT_EQ(type, Header::translateType(4, type, 3));
  }

  EXPECT_EQ(UInt8_cast(deprecated::v1::OFPT_PORT_MOD),
            Header::translateType(4, OFPT_PORT_MOD, 1));
  EXPECT_EQ(UInt8_cast(deprecated::v2::OFPT_PORT_MOD),
            Header::translateType(4, OFPT_PORT_MOD, 2));
  EXPECT_EQ(UInt8_cast(deprecated::v3::OFPT_PORT_MOD),
            Header::translateType(4, OFPT_PORT_MOD, 3));

  EXPECT_EQ(
      OFPT_PORT_MOD,
      Header::translateType(1, UInt8_cast(deprecated::v1::OFPT_PORT_MOD), 4));
  EXPECT_EQ(
      OFPT_PORT_MOD,
      Header::translateType(2, UInt8_cast(deprecated::v2::OFPT_PORT_MOD), 4));
  EXPECT_EQ(
      OFPT_PORT_MOD,
      Header::translateType(3, UInt8_cast(deprecated::v3::OFPT_PORT_MOD), 4));

  EXPECT_EQ(OFPT_UNSUPPORTED, Header::translateType(4, OFPT_SET_ASYNC, 1));
  EXPECT_EQ(OFPT_UNSUPPORTED, Header::translateType(4, OFPT_SET_ASYNC, 2));
  EXPECT_EQ(OFPT_UNSUPPORTED, Header::translateType(4, OFPT_SET_ASYNC, 3));
}
