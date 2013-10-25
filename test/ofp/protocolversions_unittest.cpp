#include "ofp/unittest.h"
#include "ofp/protocolversions.h"

using namespace ofp;

TEST(protocolversions, defaultConstructor) {
  ProtocolVersions versions;

  EXPECT_EQ(0x04, versions.highestVersion());
  EXPECT_EQ(0x1E, versions.bitmap());
  EXPECT_FALSE(versions.empty());

  EXPECT_EQ(0x04, ProtocolVersions::MaxVersion);
  EXPECT_EQ(0x1E, ProtocolVersions::All);
  EXPECT_EQ(0x00, ProtocolVersions::None);
}

TEST(protocolversions, versionConstructor) {
  ProtocolVersions versions{0x04};

  EXPECT_EQ(0x04, versions.highestVersion());
  EXPECT_EQ(0x10, versions.bitmap());
  EXPECT_FALSE(versions.empty());

  ProtocolVersions b{1, 3, 4};
}

TEST(protocolversions, fromBitmap) {
  ProtocolVersions versions = ProtocolVersions::fromBitmap(0x04);

  EXPECT_EQ(0x02, versions.highestVersion());
  EXPECT_EQ(0x04, versions.bitmap());
  EXPECT_FALSE(versions.empty());

  ProtocolVersions nothing = ProtocolVersions::fromBitmap(0);

  EXPECT_EQ(0, nothing.highestVersion());
  EXPECT_EQ(0, nothing.bitmap());
  EXPECT_TRUE(nothing.empty());
}

TEST(protocolversions, fromBitmapAll) {
  ProtocolVersions versions =
      ProtocolVersions::fromBitmap(ProtocolVersions::All);

  EXPECT_EQ(0x04, versions.highestVersion());
  EXPECT_EQ(0x1E, versions.bitmap());
  EXPECT_FALSE(versions.empty());

  ProtocolVersions other = ProtocolVersions::fromBitmap(ProtocolVersions::All);

  other = other.intersection(versions);
  EXPECT_EQ(0x1E, other.bitmap());
}
