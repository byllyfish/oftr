// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/multipartreply.h"
#include "ofp/mptablefeatures.h"

using namespace ofp;

TEST(multipartreply, mptablefeatures) {
    MemoryChannel contents{4};

    for (UInt8 i = 1; i <= 2; ++i) {
        MPTableFeaturesBuilder features;
        features.setTableId(i);
        features.setName("test");
        features.setMetadataMatch(0x2222222222222222);
        features.setMetadataWrite(0x3333333333333333);
        features.setConfig(0x44444444);
        features.setMaxEntries(0x55555555);
        features.write(&contents);
    }

    MultipartReplyBuilder msg;
    msg.setReplyType(OFPMP_TABLE_FEATURES);
    msg.setReplyFlags(0);
    msg.setReplyBody(contents.data(), contents.size());

    MemoryChannel channel{4};
    msg.send(&channel);

    EXPECT_HEX("0413009000000001000C0000000000000040010000000000746573740000000000000000000000000000000000000000000000000000000022222222222222223333333333333333444444445555555500400200000000007465737400000000000000000000000000000000000000000000000000000000222222222222222233333333333333334444444455555555", channel.data(), channel.size());

    Message message{channel.data(), channel.size()};

    const MultipartReply *m = MultipartReply::cast(&message);
    ASSERT_TRUE(m != nullptr);

    Validation context{&message};
    EXPECT_TRUE(m->validateInput(&context));
}
