// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/queuegetconfigrequest.h"

#include "ofp/unittest.h"

using namespace ofp;

TEST(queuegetconfigrequest, builderV4) {
  QueueGetConfigRequestBuilder req;

  req.setPort(0x22222222);

  MemoryChannel channel{OFP_VERSION_4};
  req.send(&channel);

  EXPECT_EQ(0x10, channel.size());
  EXPECT_HEX("04160010000000012222222200000000", channel.data(),
             channel.size());
}
