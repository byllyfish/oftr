// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/channeloptions.h"

#include "ofp/unittest.h"

using namespace ofp;

TEST(channeloptions, test) {
  EXPECT_FALSE(AreChannelOptionsValid(ChannelOptions::AUXILIARY));

  EXPECT_TRUE(AreChannelOptionsValid(ChannelOptions::AUXILIARY |
                                     ChannelOptions::FEATURES_REQ));
}
