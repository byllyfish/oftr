// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/tablemod.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(tablemod, wrongPropertySize_v5) {
  // The eviction property is supposed to be 8 bytes. This one is 16-bytes. The
  // message should be invalid.

  auto data = HexToRawData(
      "0511002000000000 FF00000000000000 0002001011223344 5566778899AABBCC");

  Message message{data.data(), data.size()};
  message.normalize();

  const TableMod *reply = TableMod::cast(&message);
  EXPECT_TRUE(reply == nullptr);
}
