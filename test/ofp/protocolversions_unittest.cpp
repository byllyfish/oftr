// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/protocolversions.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(protocolversions, defaultConstructor) {
  ProtocolVersions versions{ProtocolVersions::All};

  EXPECT_EQ(OFP_VERSION_6, versions.highestVersion());
  EXPECT_EQ(0x7E, versions.bitmap());

  std::vector<UInt8> v{1, 2, 3, 4, 5, 6};
  EXPECT_EQ(v, versions.versions());

  EXPECT_FALSE(versions.empty());

  EXPECT_EQ(0x7E, ProtocolVersions::VersionBitmapAll);
  EXPECT_EQ(0x00, ProtocolVersions::VersionBitmapNone);
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
  ProtocolVersions versions = ProtocolVersions::All;

  EXPECT_EQ(OFP_VERSION_6, versions.highestVersion());
  EXPECT_EQ(0x7E, versions.bitmap());
  EXPECT_FALSE(versions.empty());

  ProtocolVersions other = ProtocolVersions::All;

  other = other.intersection(versions);
  EXPECT_EQ(0x7E, other.bitmap());
}

TEST(protocolversions, isOnlyOneVersionSupported) {
  EXPECT_TRUE(ProtocolVersions::fromBitmap(0x01).isOnlyOneVersionSupported());
  EXPECT_TRUE(ProtocolVersions::fromBitmap(0x02).isOnlyOneVersionSupported());
  EXPECT_TRUE(ProtocolVersions::fromBitmap(0x04).isOnlyOneVersionSupported());
  EXPECT_TRUE(ProtocolVersions::fromBitmap(0x08).isOnlyOneVersionSupported());

  EXPECT_FALSE(ProtocolVersions::fromBitmap(0x00).isOnlyOneVersionSupported());
  EXPECT_FALSE(ProtocolVersions::fromBitmap(0x03).isOnlyOneVersionSupported());
  EXPECT_FALSE(ProtocolVersions::fromBitmap(0x11).isOnlyOneVersionSupported());
}

TEST(protocolversions, negotiateVersion) {
  {
    ProtocolVersions vers{1};

    EXPECT_EQ(1, vers.negotiateVersion(4, {1, 2, 3, 4}));
    EXPECT_EQ(0, vers.negotiateVersion(4, {2, 3, 4}));
    EXPECT_EQ(1, vers.negotiateVersion(3, ProtocolVersions::None));
    EXPECT_EQ(1, vers.negotiateVersion(2, ProtocolVersions::None));
    EXPECT_EQ(1, vers.negotiateVersion(1, ProtocolVersions::None));

    EXPECT_EQ(0, vers.negotiateVersion(4, {4}));
  }

  {
    ProtocolVersions vers{4};
    EXPECT_EQ(4, vers.negotiateVersion(4, {1, 2, 3, 4}));
    EXPECT_EQ(4, vers.negotiateVersion(4, ProtocolVersions::None));

    EXPECT_EQ(0, vers.negotiateVersion(3, ProtocolVersions::None));
    EXPECT_EQ(0, vers.negotiateVersion(2, ProtocolVersions::None));
    EXPECT_EQ(0, vers.negotiateVersion(1, ProtocolVersions::None));

    EXPECT_EQ(4, vers.negotiateVersion(4, {4}));
  }

  {
    ProtocolVersions vers{1, 4};
    EXPECT_EQ(4, vers.negotiateVersion(4, {1, 2, 3, 4}));
    EXPECT_EQ(4, vers.negotiateVersion(4, ProtocolVersions::None));

    EXPECT_EQ(0, vers.negotiateVersion(3, ProtocolVersions::None));
    EXPECT_EQ(0, vers.negotiateVersion(2, ProtocolVersions::None));
    EXPECT_EQ(1, vers.negotiateVersion(1, ProtocolVersions::None));

    EXPECT_EQ(4, vers.negotiateVersion(4, {4}));
  }

  {
    ProtocolVersions vers{2, 3, 4};
    EXPECT_EQ(4, vers.negotiateVersion(4, {1, 2, 3, 4}));
    EXPECT_EQ(4, vers.negotiateVersion(4, ProtocolVersions::None));

    EXPECT_EQ(3, vers.negotiateVersion(3, ProtocolVersions::None));
    EXPECT_EQ(2, vers.negotiateVersion(2, ProtocolVersions::None));
    EXPECT_EQ(0, vers.negotiateVersion(1, ProtocolVersions::None));

    EXPECT_EQ(4, vers.negotiateVersion(4, {4}));
  }

  {
    ProtocolVersions vers{2, 3};
    EXPECT_EQ(3, vers.negotiateVersion(4, {1, 2, 3, 4}));
    EXPECT_EQ(3, vers.negotiateVersion(4, ProtocolVersions::None));

    EXPECT_EQ(3, vers.negotiateVersion(3, ProtocolVersions::None));
    EXPECT_EQ(2, vers.negotiateVersion(2, ProtocolVersions::None));
    EXPECT_EQ(0, vers.negotiateVersion(1, ProtocolVersions::None));

    EXPECT_EQ(0, vers.negotiateVersion(4, {4}));
  }
}

TEST(protocolversions, toString) {
  ProtocolVersions v1 = {1, 3, 4};
  EXPECT_EQ("[1, 3, 4]", v1.toString());

  ProtocolVersions v2 = {};
  EXPECT_EQ("[]", v2.toString());

  ProtocolVersions v3 = {3};
  EXPECT_EQ("[3]", v3.toString());
}
