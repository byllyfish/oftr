// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/vlannumber.h"
#include "ofp/unittest.h"

using namespace ofp;

OFP_BEGIN_IGNORE_PADDING

struct VlanTestInfo {
  SignedInt32 code;
  UInt16 value;
};

OFP_END_IGNORE_PADDING

static VlanTestInfo sVlanTestInfo[] = {
    {-0xffff, 0xffff},
    {-0xefff, 0xefff},
    {-8193, 8193},
    {-8192, 8192},
    // [-8191, -4096] do not roundtrip.
    {-4095, 4095},
    {-4094, 4094},
    {-2, 2},
    {-1, 1},
    {0, 0},
    {1, OFPVID_PRESENT + 1},
    {2, OFPVID_PRESENT + 2},
    {4095, OFPVID_PRESENT + 4095},
    {4096, OFPVID_PRESENT},
    // [4097...] do not roundtrip.
};

TEST(vlannumber, test) {
  VlanNumber vlan;

  for (const auto &info : sVlanTestInfo) {
    vlan.setDisplayCode(info.code);
    EXPECT_EQ(info.value, vlan);
    EXPECT_EQ(info.code, vlan.displayCode());
  }

  // Display code values outside [-4095, 4096] do *not* roundtrip.

  vlan.setDisplayCode(4097);
  EXPECT_EQ(4097, vlan);
  EXPECT_EQ(1, vlan.displayCode());

  vlan.setDisplayCode(4098);
  EXPECT_EQ(4098, vlan);
  EXPECT_EQ(2, vlan.displayCode());

  vlan.setDisplayCode(-4096);
  EXPECT_EQ(4096, vlan);
  EXPECT_EQ(4096, vlan.displayCode());

  vlan.setDisplayCode(-4097);
  EXPECT_EQ(4097, vlan);
  EXPECT_EQ(1, vlan.displayCode());

  vlan.setDisplayCode(-8191);
  EXPECT_EQ(8191, vlan);
  EXPECT_EQ(4095, vlan.displayCode());

  vlan.setDisplayCode(8191);
  EXPECT_EQ(8191, vlan);
  EXPECT_EQ(4095, vlan.displayCode());

  vlan.setDisplayCode(0xefff);
  EXPECT_EQ(8191, vlan);
  EXPECT_EQ(4095, vlan.displayCode());

  vlan.setDisplayCode(-0xefff);
  EXPECT_EQ(0xefff, vlan);
  EXPECT_EQ(-0xefff, vlan.displayCode());
}
