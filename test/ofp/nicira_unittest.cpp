#include "ofp/unittest.h"
#include "ofp/nicira.h"

using namespace ofp;

TEST(nicira, AT_REGMOVE) {
    OXMRegister src{"ETH_DST[1:16]"};
    OXMRegister dst{"IPV4_SRC[2:17]"};
    nx::AT_REGMOVE act{src, dst};

    EXPECT_HEX("FFFF0018000023200006000F000100028000060680001604", &act, sizeof(act));
}
