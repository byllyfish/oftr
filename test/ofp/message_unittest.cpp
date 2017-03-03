// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/message.h"
#include "ofp/flowmod.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(message, constructor) {
  Message message{nullptr};
  EXPECT_EQ(8, message.size());
}

TEST(message, normalize) {
  const char *hexBefore =
      "010E 0048 0000 002C 0010 001F 0000 0000 0000 0000 0000 0000 0000 0000 "
      "0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 "
      "0003 0000 0000 8000 FFFF FFFF FFFF 0000";
  auto s = HexToRawData(hexBefore);

  Message msg{nullptr};
  std::memcpy(msg.mutableData(s.length()), s.data(), s.length());

  EXPECT_EQ(0x48, msg.size());
  EXPECT_HEX(hexBefore, msg.data(), msg.size());

  msg.normalize();

  const char *hexAfter =
      "010E00880000002C0000000000000000FFFFFFFFFFFFFFFF0003000000008000FFFFFFFF"
      "FFFFFFFF000000000000000000000058000000000000030F000000000000000000000000"
      "000000000000000000000000000000000000000000000000FFFFFFFF00000000FFFFFFFF"
      "00000000000000000000000000000000000000000000000000000000";

  EXPECT_EQ(0x088, msg.size());
  EXPECT_HEX(hexAfter, msg.data(), msg.size());
}

TEST(message, normalize_flow_mod) {
  const char *hexBefore =
      "010E 0048 0000 0060 0010 001F 0000 0000 0000 0000 "
      "0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 "
      "0000 0000 0000 0000 0000 0000 0000 0000 0003 0000 "
      "0000 8000 FFFF FFFF FFFF 0000";

  auto s = HexToRawData(hexBefore);

  Message message{nullptr};
  std::memcpy(message.mutableData(s.length()), s.data(), s.length());

  message.normalize();

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
