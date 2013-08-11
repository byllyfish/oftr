#include "ofp/unittest.h"
#include "ofp/protocolversions.h"

using namespace ofp;

TEST(protocolversions, defaultConstructor) 
{
	ProtocolVersions versions;

	EXPECT_EQ(0x04, versions.highestVersion());
	EXPECT_EQ(0x1E, versions.bitmap());

	EXPECT_EQ(0x04, ProtocolVersions::MaxVersion);
	EXPECT_EQ(0x1E, ProtocolVersions::All);
	EXPECT_EQ(0x00, ProtocolVersions::None);
}


TEST(protocolversions, bitmapConstructor) 
{
	ProtocolVersions versions{0x04};

	EXPECT_EQ(0x02, versions.highestVersion());
	EXPECT_EQ(0x04, versions.bitmap());
}

