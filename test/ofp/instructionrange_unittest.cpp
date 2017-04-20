#include "ofp/unittest.h"
#include "ofp/instructionrange.h"
#include "ofp/validation.h"

using namespace ofp;

TEST(instructionrange, test) {
    ByteList buf{HexToRawData("00030028000000000019001080000c0201020000000000000000001000000006ffff000000000000")};

    Validation context;
    InstructionRange range = buf.toRange();
    EXPECT_TRUE(range.validateInput(&context));

    buf.mutableData()[15] = 0xff;
    EXPECT_FALSE(range.validateInput(&context));
}
