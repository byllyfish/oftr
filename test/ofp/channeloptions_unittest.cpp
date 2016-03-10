#include "ofp/unittest.h"
#include "ofp/channeloptions.h"

using namespace ofp;

TEST(channeloptions, test) {
    EXPECT_FALSE(AreChannelOptionsValid(ChannelOptions::AUXILIARY));
    EXPECT_FALSE(AreChannelOptionsValid(ChannelOptions::LISTEN_UDP));
    EXPECT_FALSE(AreChannelOptionsValid(ChannelOptions::CONNECT_UDP | ChannelOptions::FEATURES_REQ));

    EXPECT_TRUE(AreChannelOptionsValid(ChannelOptions::AUXILIARY | ChannelOptions::FEATURES_REQ));
    EXPECT_TRUE(AreChannelOptionsValid(ChannelOptions::LISTEN_UDP | ChannelOptions::AUXILIARY | ChannelOptions::FEATURES_REQ));
    EXPECT_TRUE(AreChannelOptionsValid(ChannelOptions::CONNECT_UDP));
}
