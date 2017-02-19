// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/actionrange.h"
#include "ofp/bucketrange.h"
#include "ofp/instructionrange.h"
#include "ofp/meterbandrange.h"
#include "ofp/packetcounterrange.h"
#include "ofp/port.h"
#include "ofp/queuerange.h"
#include "ofp/unittest.h"
#include "ofp/validation.h"

using namespace ofp;

#define TEST_INVALID_RANGE(Clazz, Str)           \
  TEST(validateinput, Clazz##_invalid) {         \
    ByteList buf{HexToRawData(Str)};             \
    Clazz range{buf};                            \
    Validation context;                          \
    EXPECT_FALSE(range.validateInput(&context)); \
  }

#define TEST_VALID_RANGE(Clazz, Str)            \
  TEST(validateinput, Clazz##_valid) {          \
    ByteList buf{HexToRawData(Str)};            \
    Clazz range{buf};                           \
    Validation context;                         \
    EXPECT_TRUE(range.validateInput(&context)); \
  }

#define TEST_VALID_STRUCT(Clazz, Str)                               \
  TEST(validateinput, Clazz##_valid) {                              \
    ByteList buf{HexToRawData(Str)};                                \
    const Clazz *val = reinterpret_cast<const Clazz *>(buf.data()); \
    Validation context;                                             \
    context.setLengthRemaining(buf.size());                         \
    EXPECT_TRUE(val->validateInput(&context));                      \
  }

TEST_INVALID_RANGE(ActionRange, "1111001001020304")
TEST_VALID_RANGE(ActionRange, "1111000801020304")

TEST_INVALID_RANGE(InstructionRange, "1111001001020304")
TEST_VALID_RANGE(InstructionRange, "1111000801020304")

TEST_INVALID_RANGE(BucketRange, "1111001001020304")
TEST_VALID_RANGE(BucketRange, "001011110102030405060708090a0b0c")

TEST_INVALID_RANGE(MeterBandRange, "1111001001020304")
TEST_VALID_RANGE(MeterBandRange, "1111000801020304")

TEST_INVALID_RANGE(PacketCounterRange, "1111001001020304")
TEST_VALID_RANGE(PacketCounterRange, "000102030405060708090a0b0c0d0e0f")

TEST_INVALID_RANGE(QueueRange, "1111001001020304")
TEST_VALID_RANGE(QueueRange, "11111111222222220010010203040506")

TEST_VALID_STRUCT(Port,
                  "1111111100280000 0102030405060708 0102030405060708 "
                  "0102030405060708 0102030405060708")
