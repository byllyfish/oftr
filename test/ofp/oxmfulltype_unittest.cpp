#include "ofp/unittest.h"
#include "ofp/oxmfulltype.h"

using namespace ofp;

TEST(oxmfulltype, constructor) {
    OXMFullType type;
    EXPECT_EQ(0, type.type().oxmNative());
    EXPECT_EQ(0, type.experimenter());
    EXPECT_EQ(OXMInternalID::UNKNOWN, type.internalID());
    EXPECT_FALSE(type.hasMask());
}

TEST(oxmfulltype, x_lldp_chassis_id) {
    OXMFullType type;
    EXPECT_TRUE(type.parse("X_LLDP_CHASSIS_ID"));

    EXPECT_EQ(0xFFFF0244, type.type().oxmNative());
    EXPECT_EQ(0x00FFFFFF, type.experimenter());
    EXPECT_EQ(OXMInternalID::X_LLDP_CHASSIS_ID, type.internalID());
    EXPECT_FALSE(type.hasMask());
}

TEST(oxmfulltype, x_experimenter_01) {
    OXMFullType type{X_EXPERIMENTER_01::type(), X_EXPERIMENTER_01::experimenter()};

    EXPECT_EQ(OXMInternalID::X_EXPERIMENTER_01, type.internalID());
}
