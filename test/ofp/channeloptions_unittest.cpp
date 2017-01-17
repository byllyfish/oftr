// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/channeloptions.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(channeloptions, test) {
  EXPECT_FALSE(AreChannelOptionsValid(ChannelOptions::AUXILIARY));
  EXPECT_FALSE(AreChannelOptionsValid(ChannelOptions::LISTEN_UDP));
  EXPECT_FALSE(AreChannelOptionsValid(ChannelOptions::CONNECT_UDP |
                                      ChannelOptions::FEATURES_REQ));

  EXPECT_TRUE(AreChannelOptionsValid(ChannelOptions::AUXILIARY |
                                     ChannelOptions::FEATURES_REQ));
  EXPECT_TRUE(AreChannelOptionsValid(ChannelOptions::LISTEN_UDP |
                                     ChannelOptions::AUXILIARY |
                                     ChannelOptions::FEATURES_REQ));
  EXPECT_TRUE(AreChannelOptionsValid(ChannelOptions::CONNECT_UDP));
}
