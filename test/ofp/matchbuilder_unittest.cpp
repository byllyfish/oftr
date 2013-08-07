#include "ofp/unittest.h"
#include "ofp/matchbuilder.h"

using namespace ofp;

TEST(matchbuilder, OFB_IN_PORT) 
{
	MatchBuilder match;

	match.add(OFB_IN_PORT{7});

	EXPECT_HEX("8000 0004 00000007", match.data(), match.size());
	EXPECT_TRUE(match.validate());

	// Following line should not compile; OFB_IN_PORT doesn't support a mask.
	// match.add(OFB_IN_PORT{9}, OFB_IN_PORT{0xFF});
}

TEST(matchbuilder, OFB_IN_PHY_PORT) 
{
	MatchBuilder match;

	match.add(OFB_IN_PORT{7});
	match.add(OFB_IN_PHY_PORT{9});

	EXPECT_HEX("8000 0004 00000007 8000-0204-00000009", match.data(), match.size());
	EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_IN_PHY_PORT_alt1) 
{
	MatchBuilder match;

	match.add(OFB_IN_PHY_PORT{9});

	// We should have the prereq placeholder for OFB_IN_PORT added.
	EXPECT_HEX("7FF1-0100 8000-0108-00000000-00000000 8000-0204-00000009", match.data(), match.size());
	EXPECT_FALSE(match.validate());

	match.add(OFB_IN_PORT{7});

	// The prereq for OFB_IN_PHY_PORT should replace the placeholder.
	EXPECT_HEX("8000-0004-00000007 8000-0204-00000009", match.data(), match.size());
	EXPECT_TRUE(match.validate());
}


TEST(matchbuilder, OFB_VLAN_VID) 
{
	MatchBuilder match;

	// FIXME: should we allow VID to be set without OFPVID_PRESENT?
	match.add(OFB_VLAN_VID{15});
	EXPECT_HEX("8000-0C02-000F", match.data(), match.size());
	EXPECT_TRUE(match.validate());

	match.add(OFB_VLAN_PCP{1});

	// The current VLAN_ID does not satisfy the prerequisite; it is missing the
	// OFPVID_PRESENT bit. Therefore, we should see the prerequisite fail to 
	// match the OFB_VLAN_VID oxm -- there should be a placeholder for the preq.
	
	EXPECT_HEX("8000-0C02-000F 7FF1-0100 8000-0D04-10001000 8000-0E01-01", match.data(), match.size());
	EXPECT_FALSE(match.validate());
}


TEST(matchbuilder, OFB_VLAN_VID_alt1) 
{
	MatchBuilder match;

	match.add(OFB_VLAN_VID{15 | OFPVID_PRESENT});
	EXPECT_HEX("8000-0C02-100F", match.data(), match.size());
	EXPECT_TRUE(match.validate());

	match.add(OFB_VLAN_PCP{1});
	EXPECT_HEX("8000-0C02-100F 8000-0E01-01", match.data(), match.size());
	EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_VLAN_VID_alt2) 
{
	MatchBuilder match;

	match.add(OFB_VLAN_PCP{2});
	EXPECT_HEX("7FF1-0100 8000-0D04-10001000 8000-0E01-02", match.data(), match.size());
	EXPECT_FALSE(match.validate());

	match.add(OFB_VLAN_VID{15 | OFPVID_PRESENT});
	EXPECT_HEX("8000-0C02-100F 8000-0E01-02", match.data(), match.size());
	EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_VLAN_VID_alt3) 
{
	MatchBuilder match;

	match.add(OFB_VLAN_PCP{2});
	EXPECT_HEX("7FF1-0100 8000-0D04-10001000 8000-0E01-02", match.data(), match.size());
	EXPECT_FALSE(match.validate());

	match.add(OFB_VLAN_VID{15});

	// As in the original test for OFB_VLAN_VID, the VLAN_VID field does not 
	// satisfy the prerequisite, so the placeholder is appended.

	EXPECT_HEX("7FF1-0100 8000-0D04-10001000 8000-0E01-02 8000-0C02-000F", match.data(), match.size());
	EXPECT_FALSE(match.validate());

	match.add(OFB_VLAN_VID{5 | OFPVID_PRESENT});

	EXPECT_HEX("8000-0C02-1005 8000-0E01-02 8000-0C02-000F", match.data(), match.size());
	EXPECT_FALSE(match.validate());
}
