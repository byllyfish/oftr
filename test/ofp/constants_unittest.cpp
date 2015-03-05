#include "ofp/unittest.h"
#include "ofp/constants.h"

using namespace ofp;
using namespace deprecated;

static std::pair<UInt32, UInt32> actionTypes[] = {
    {OFPATF_OUTPUT, 1 << v1::OFPAT_OUTPUT},
    {OFPATF_SET_VLAN_VID, 1 << v1::OFPAT_SET_VLAN_VID},
    {OFPATF_SET_VLAN_PCP, 1 << v1::OFPAT_SET_VLAN_PCP},
    {OFPATF_STRIP_VLAN_V1, 1 << v1::OFPAT_STRIP_VLAN},
    {
     OFPATF_SET_DL_SRC, 1 << v1::OFPAT_SET_DL_SRC,
    },
    {OFPATF_SET_DL_DST, 1 << v1::OFPAT_SET_DL_DST},
    {OFPATF_SET_NW_SRC, 1 << v1::OFPAT_SET_NW_SRC},
    {OFPATF_SET_NW_DST, 1 << v1::OFPAT_SET_NW_DST},
    {OFPATF_SET_NW_TOS, 1 << v1::OFPAT_SET_NW_TOS},
    {OFPATF_SET_NW_ECN, 0},
    {OFPATF_SET_TP_SRC, 1 << v1::OFPAT_SET_TP_SRC},
    {OFPATF_SET_TP_DST, 1 << v1::OFPAT_SET_TP_DST},
    {OFPATF_ENQUEUE_V1, 1 << v1::OFPAT_ENQUEUE},
    {OFPATF_COPY_TTL_OUT, 0},
    {OFPATF_COPY_TTL_IN, 0},
    {OFPATF_SET_MPLS_LABEL, 0},
    {OFPATF_SET_MPLS_TC, 0},
    {OFPATF_SET_MPLS_TTL, 0},
    {OFPATF_DEC_MPLS_TTL, 0},
    {OFPATF_PUSH_VLAN, 0},
    {OFPATF_POP_VLAN, 0},
    {OFPATF_PUSH_MPLS, 0},
    {OFPATF_POP_MPLS, 0},
    {OFPATF_SET_QUEUE, 0},
    {OFPATF_GROUP, 0},
    {OFPATF_SET_NW_TTL, 0},
    {OFPATF_DEC_NW_TTL, 0},
    {OFPATF_SET_FIELD, 0},
    {OFPATF_PUSH_PBB, 0},
    {OFPATF_POP_PBB, 0},
};

TEST(constants, OFPActionTypeFlagsConvertToV1) {
  EXPECT_EQ(0, OFPActionTypeFlagsConvertToV1(0));
  EXPECT_EQ(0x01F7, OFPActionTypeFlagsConvertToV1(0xFF));
  EXPECT_EQ(0x07F7, OFPActionTypeFlagsConvertToV1(0xFFFF));
  EXPECT_EQ(0x07F7, OFPActionTypeFlagsConvertToV1(0xFFFFFF));
  EXPECT_EQ(0x0FFF, OFPActionTypeFlagsConvertToV1(0xFFFFFFFF));
  EXPECT_EQ(0x06EF, OFPActionTypeFlagsConvertToV1(0x77777777));
  EXPECT_EQ(0x0777, OFPActionTypeFlagsConvertToV1(0x06BF));

  for (size_t i = 0; i < ArrayLength(actionTypes); ++i) {
    EXPECT_EQ(actionTypes[i].second,
              OFPActionTypeFlagsConvertToV1(actionTypes[i].first))
        << "Failed at iteration " << i;
  }
}

TEST(constants, OFPActionTypeFlagsConvertFromV1) {
  EXPECT_EQ(0, OFPActionTypeFlagsConvertFromV1(0));
  EXPECT_EQ(0x4000007F, OFPActionTypeFlagsConvertFromV1(0xFF));
  EXPECT_EQ(0xC00006FF, OFPActionTypeFlagsConvertFromV1(0x0FFF));
  EXPECT_EQ(0xC00006FF, OFPActionTypeFlagsConvertFromV1(0xFFFF));
  EXPECT_EQ(0xC00006FF, OFPActionTypeFlagsConvertFromV1(0xFFFFFF));
  EXPECT_EQ(0xC00006FF, OFPActionTypeFlagsConvertFromV1(0xFFFFFFFF));
  EXPECT_EQ(0x06BF, OFPActionTypeFlagsConvertFromV1(0x77777777));

  for (size_t i = 0; i < ArrayLength(actionTypes); ++i) {
    if (actionTypes[i].second) {
      EXPECT_EQ(actionTypes[i].first,
                OFPActionTypeFlagsConvertFromV1(actionTypes[i].second))
          << "Failed at iteration " << i;
    }
  }
}

TEST(constants, OFPCapabilitiesFlagsConvertToV1) {
  EXPECT_EQ(0, OFPCapabilitiesFlagsConvertToV1(0));
  EXPECT_EQ(0x7fffffff, OFPCapabilitiesFlagsConvertToV1(0xFFFFFFFF));
  EXPECT_EQ(deprecated::v1::OFPC_STP,
            OFPCapabilitiesFlagsConvertToV1(OFPC_STP));
}

TEST(constants, OFPCapabilitiesFlagsConvertFromV1) {
  EXPECT_EQ(0, OFPCapabilitiesFlagsConvertFromV1(0));
  EXPECT_EQ(0xfffffff7, OFPCapabilitiesFlagsConvertFromV1(0xFFFFFFFF));
  EXPECT_EQ(OFPC_STP,
            OFPCapabilitiesFlagsConvertFromV1(deprecated::v1::OFPC_STP));
}

TEST(constants, OFPPortFeaturesFlagsConvertToV1) {
  EXPECT_EQ(0, OFPPortFeaturesFlagsConvertToV1(0));
  EXPECT_EQ(0xffff0fff, OFPPortFeaturesFlagsConvertToV1(0xFFFFFFFF));
  EXPECT_EQ(deprecated::v1::OFPPF_COPPER,
            OFPPortFeaturesFlagsConvertToV1(OFPPF_COPPER));
  EXPECT_EQ(deprecated::v1::OFPPF_PAUSE_ASYM,
            OFPPortFeaturesFlagsConvertToV1(OFPPF_PAUSE_ASYM));
}

TEST(constants, OFPPortFeaturesFlagsConvertFromV1) {
  EXPECT_EQ(0, OFPPortFeaturesFlagsConvertFromV1(0));
  EXPECT_EQ(0xfffff87f, OFPPortFeaturesFlagsConvertFromV1(0xFFFFFFFF));
  EXPECT_EQ(OFPPF_COPPER,
            OFPPortFeaturesFlagsConvertFromV1(deprecated::v1::OFPPF_COPPER));
  EXPECT_EQ(OFPPF_PAUSE_ASYM, OFPPortFeaturesFlagsConvertFromV1(
                                  deprecated::v1::OFPPF_PAUSE_ASYM));
}
