// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/hello.h"

using namespace ofp;

TEST(hello, HelloBuilder1) {
  HelloBuilder msg{ProtocolVersions{1, 2, 3, 4}};

  MemoryChannel channel{99};
  msg.send(&channel);

  EXPECT_EQ(0x10, channel.size());
  EXPECT_HEX("0400-0010-00000001 00010008-0000001E", channel.data(),
             channel.size());
}

TEST(hello, HelloBuilder2) {
  HelloBuilder msg{ProtocolVersions{1, 4}};

  MemoryChannel channel{OFP_VERSION_1};
  msg.send(&channel);

  EXPECT_EQ(0x10, channel.size());
  EXPECT_HEX("0400-0010-00000001 00010008-00000012", channel.data(),
             channel.size());
}

TEST(hello, HelloBuilder3) {
  HelloBuilder msg{ProtocolVersions{1}};

  MemoryChannel channel{OFP_VERSION_4};
  msg.send(&channel);

  EXPECT_EQ(0x08, channel.size());
  EXPECT_HEX("0100-0008-00000001", channel.data(), channel.size());
}

TEST(hello, HelloBuilder4) {
  HelloBuilder msg{OFP_VERSION_1};

  MemoryChannel channel{0};
  msg.send(&channel);

  EXPECT_EQ(0x08, channel.size());
  EXPECT_HEX("0100-0008-00000001", channel.data(), channel.size());
}
