// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/tablemod.h"

using namespace ofp;

TEST(tablemod, wrongPropertySize_v5) {
  // The eviction property is supposed to be 8 bytes. This one is 16-bytes. The
  // message should be invalid.

  auto data = HexToRawData(
      "0511002000000000 FF00000000000000 0002001011223344 5566778899AABBCC");

  Message message{data.data(), data.size()};
  message.transmogrify();

  const TableMod *reply = TableMod::cast(&message);
  EXPECT_TRUE(reply == nullptr);
}
