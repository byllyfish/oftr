// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/actionrange.h"

#include "ofp/actionlist.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(actionrange, validateInput_regMove) {
  // nx::AT_REGMOVE(src, dst).
  ByteList data{
      HexToRawData("FFFF00180000232000060020000000008000160480001804")};
  ActionRange range{data};

  Validation context;
  EXPECT_TRUE(range.validateInput(&context));
}

TEST(actionrange, validateInput_regMove_fail) {
  // nx::AT_REGMOVE(src, dst) (invalid; wrong size).
  ByteList data{HexToRawData(
      "FFFF002000002320000600200000000080001604800018040000000000000000")};
  ActionRange range{data};

  Validation context;
  EXPECT_FALSE(range.validateInput(&context));
}
