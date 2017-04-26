// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/instructionrange.h"
#include "ofp/unittest.h"
#include "ofp/validation.h"

using namespace ofp;

TEST(instructionrange, test) {
  ByteList buf{
      HexToRawData("00030028000000000019001080000c02010200000000000000000010000"
                   "00006ffff000000000000")};

  Validation context;
  InstructionRange range = buf.toRange();
  EXPECT_TRUE(range.validateInput(&context));

  buf.mutableData()[15] = 0xff;
  EXPECT_FALSE(range.validateInput(&context));
}
