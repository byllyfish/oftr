// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/nicira.h"

#include "ofp/unittest.h"

using namespace ofp;

TEST(nicira, AT_REGMOVE) {
  OXMRegister src{"ETH_DST[1:16]"};
  OXMRegister dst{"IPV4_SRC[2:17]"};
  nx::AT_REGMOVE act{src, dst};

  EXPECT_HEX("FFFF0018000023200006000F000100028000060680001604", &act,
             sizeof(act));
}

TEST(nicira, AT_REGLOAD) {
  OXMRegister dst{"IPV4_SRC[2:17]"};
  nx::AT_REGLOAD act{0x12, dst};

  EXPECT_HEX("FFFF0018000023200007008E800016040000000000000012", &act,
             sizeof(act));
}
