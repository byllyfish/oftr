// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmrange.h"

#include "ofp/oxmvalue.h"
#include "ofp/unittest.h"

using namespace ofp;

namespace {

constexpr OXMInternalID cast(int n) {
  return static_cast<OXMInternalID>(n);
}

using OFB_InPort = OXMValue<cast(0), 0x8000, 0, Big16, 2, false>;
using OFB_VlanVid = OXMValue<cast(1), 0x8000, 6, Big16, 2, true>;
using OFB_TCPSrcPort = OXMValue<cast(2), 0x8000, 19, Big16, 2, false>;

const char *buffer =
    "8000 0002 0100"
    "8000 0c02 FFFF"
    "8000 2602 0064";

}  // namespace

TEST(oxmrange, test) {
  auto buf = HexToRawData(buffer);
  OXMRange range{buf.data(), buf.size()};

  UInt16 port = 0;
  for (auto &item : range) {
    switch (item.type()) {
      case OFB_TCPSrcPort::type():
        port = item.value<OFB_TCPSrcPort>();
        break;
    }
  }

  EXPECT_EQ(100, port);
}

TEST(oxmrange, validateInput_invalid_1) {
  auto buf = HexToRawData(
      "8000050FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8000090CFFFFFFFFFFFFFFFFFFFFFFFF"
      "80000806FFFFFFFFFFFF80000510FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF80000D04FFFF"
      "FFFF80000D04FFFFFFFF");
  OXMRange range{buf.data(), buf.size()};

  EXPECT_FALSE(range.validateInput());
}

TEST(oxmrange, validateInput_short_experimenter) {
  // Experimenter OXM's do not have to include 4 bytes for the experimenter
  // code.
  auto buf = HexToRawData("FFFF000300FFFF");

  OXMRange range{buf.data(), buf.size()};

  EXPECT_TRUE(range.validateInput());
}

TEST(oxmrange, validateInput_zeroField) {
  auto buf = HexToRawData("00000000");

  OXMRange range{buf.data(), buf.size()};
  EXPECT_TRUE(range.validateInput());
}

TEST(oxmrange, iteratate_zeroField) {
  auto buf = HexToRawData(
      "80000A020800800016040A0A0A0180001401018000280100800026010000000000");
  OXMRange range{buf.data(), buf.size()};
  EXPECT_TRUE(range.validateInput());

  for (const auto &item : range) {
    EXPECT_FALSE(item.type().isIllegal());
  }
}

TEST(oxmrange, find_in_invalid_range) {
  auto buf = HexToRawData(
      "800020240000000680000aa0080680000606ffffffffbfff80000806f20ba47df8ea8000"
      "2a02000180002c140a00000180002e040a00000300003006f20ba47df8ea800032060000"
      "000084040000fffffffffffff20ba47d");

  OXMRange range{buf.data(), buf.size()};
  EXPECT_FALSE(range.validateInput());
  EXPECT_EQ(0, range.get<OFB_InPort>());
  EXPECT_FALSE(range.exists<OFB_InPort>());
}

TEST(oxmrange, validateInput_paddedField) {
  auto buf1 = HexToRawData("80000A02 ffff 00000000000000");

  OXMRange range{buf1.data(), buf1.size()};
  EXPECT_TRUE(range.validateInput_paddedField());
  EXPECT_FALSE(range.validateInput());
}
