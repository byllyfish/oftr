// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/protocolrange.h"

#include "ofp/unittest.h"
#include "ofp/validation.h"

using namespace ofp;
using namespace ofp::detail;

TEST(protocolrange, valid_empty) {
  OFPErrorCode error;

  ByteRange r1;
  Validation context1{nullptr, &error};
  EXPECT_TRUE(IsProtocolRangeValid(4, r1, 2, 8, &context1));

  UInt64 d2 = 0;
  ByteRange r2{&d2, static_cast<size_t>(0)};
  Validation context2{nullptr, &error};
  EXPECT_TRUE(IsProtocolRangeValid(4, r2, 2, 8, &context2));
}

TEST(protocolrange, invalid_empty) {
  OFPErrorCode error;
  UInt8 d1[2];
  ByteRange r1{&d1[1], static_cast<size_t>(0)};
  Validation context1{nullptr, &error};
  EXPECT_FALSE(IsProtocolRangeValid(4, r1, 2, 8, &context1));
}

TEST(protocolrange, invalid_short) {
  OFPErrorCode error;

  Big16 d1 = 0xff01;
  Big64 d2 = 0xffff0001ffffffff;
  Big64 d3 = 0xffff0000ffffffff;

  ByteRange r1{&d1, 2};
  ByteRange r2{&d2, 8};
  ByteRange r3{&d3, 8};

  Validation c1{nullptr, &error};

  EXPECT_FALSE(IsProtocolRangeValid(4, r1, 2, 8, &c1));
  EXPECT_FALSE(IsProtocolRangeValid(4, r2, 2, 8, &c1));
  EXPECT_FALSE(IsProtocolRangeValid(4, r3, 2, 8, &c1));
}

TEST(protocolrange, valid_short) {
  OFPErrorCode error;

  Big64 d1 = 0xffff0004ffffffff;
  Big64 d2 = 0xffff0005ffffffff;
  Big64 d3 = 0xffff0007ffffffff;

  ByteRange r1{&d1, 8};
  ByteRange r2{&d2, 8};
  ByteRange r3{&d3, 8};

  Validation c1{nullptr, &error};

  EXPECT_TRUE(IsProtocolRangeValid(4, r1, 2, 8, &c1));
  EXPECT_TRUE(IsProtocolRangeValid(4, r2, 2, 8, &c1));
  EXPECT_TRUE(IsProtocolRangeValid(4, r3, 2, 8, &c1));
}

TEST(protocolrange, valid_exact) {
  OFPErrorCode error;
  Big64 d1 = 0xffff0008ffffffff;
  ByteRange r1{&d1, 8};
  Validation c1{nullptr, &error};

  EXPECT_TRUE(IsProtocolRangeValid(4, r1, 2, 8, &c1));
}

TEST(protocolrange, invalid1) {
  // Need to convert to ByteList for alignment; string data not necessarily
  // aligned to an 8-byte boundary.

  ByteList r1{HexToRawData("FFFFFFFFFFFFFFFFFF")};
  ByteList r2{HexToRawData("FFFF0009FFFFFFFF")};
  ByteList r3{HexToRawData("FFFFFFFFFFFFFFFF")};

  OFPErrorCode error;
  Validation c1{nullptr, &error};

  EXPECT_FALSE(IsProtocolRangeValid(4, r1, 2, 8, &c1));
  EXPECT_FALSE(IsProtocolRangeValid(4, r2, 2, 8, &c1));
  EXPECT_FALSE(IsProtocolRangeValid(4, r3, 2, 8, &c1));
}

TEST(protocolrange, invalid2) {
  ByteList r1{HexToRawData("ffff0008ffffffffff")};
  ByteList r2{HexToRawData("ffff0006ffffffffffffffffffffffff")};

  OFPErrorCode error;
  Validation c1{nullptr, &error};

  EXPECT_FALSE(IsProtocolRangeValid(4, r1, 2, 8, &c1));
  EXPECT_FALSE(IsProtocolRangeValid(4, r2, 2, 8, &c1));
}

TEST(protocolrange, iteration) {
  Big64 data[] = {0xffff000800000001, 0xffff000800000002, 0xffff000800000003,
                  0xffff000800000004};

  struct Item : private NonCopyable {
    enum { ProtocolIteratorSizeOffset = 2, ProtocolIteratorAlignment = 8 };
    Big16 type;
    Big16 len;
    Big32 value;

    bool validateInput(Validation *context) const { return true; }
  };

  ByteRange r1{data, sizeof(data)};
  OFPErrorCode error;
  Validation c1{nullptr, &error};
  EXPECT_TRUE(IsProtocolRangeValid(sizeof(Item), r1, 2, 8, &c1,
                                   ProtocolIteratorType::Unspecified));

  ProtocolRange<ProtocolIterator<Item>> iterable{r1};
  EXPECT_TRUE(iterable.validateInput(&c1));
  EXPECT_EQ(4, iterable.itemCount());

  unsigned sum = 10;

  // Two ways to iterate:

  // (1) Use a const reference.
  for (auto &elem : iterable) {
    sum += elem.value;
  }
  EXPECT_EQ(20, sum);

  // (2) Iterate using pointers.
  for (auto iter = iterable.begin(); iter != iterable.end(); ++iter) {
    sum += iter->value;
    sum += (*iter).value;
  }
  EXPECT_EQ(40, sum);

  // Wrong: Assignment in a for loop doesn't compile...
  // for (auto elem : iterable) {
  //   sum += elem.value;
  // }
}

TEST(protocolrange, misalignedSeq) {
  ByteList r1{HexToRawData(
      "00 01 00 05 00 10 00 04 00 02 00 04 00 13 00 04 00 02 00 04 00 10 00 04 "
      "00 18 00 04 00 09 00 04 00 06 00 04 00 03 00 04")};

  struct Item : private NonCopyable {
    enum { ProtocolIteratorSizeOffset = 2, ProtocolIteratorAlignment = 4 };
    Big16 type;
    Big16 len;
    Big32 data;
    bool validateInput(Validation *context) const { return true; }
  };

  ProtocolRange<ProtocolIterator<Item>> iterable{r1};

  // Sequence is invalid because first item has len of 5 which is misaligned.
  OFPErrorCode error;
  Validation context{nullptr, &error};
  EXPECT_FALSE(iterable.validateInput(&context));
}
