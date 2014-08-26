#include "ofp/unittest.h"
#include "ofp/header.h"

using namespace ofp;

TEST(header, translateType) {
  for (UInt8 type = 0; type <= OFPT_LAST; ++type) {
    EXPECT_EQ(type, Header::translateType(4, type, 4));
  }

  for (UInt8 type = 0; type <= OFPT_FLOW_MOD; ++type) {
    EXPECT_EQ(type, Header::translateType(1, type, 4));
    EXPECT_EQ(type, Header::translateType(2, type, 4));
    EXPECT_EQ(type, Header::translateType(3, type, 4));
    EXPECT_EQ(type, Header::translateType(4, type, 1));
    EXPECT_EQ(type, Header::translateType(4, type, 2));
    EXPECT_EQ(type, Header::translateType(4, type, 3));
  }

  EXPECT_EQ(UInt8_cast(deprecated::v1::OFPT_PORT_MOD),
            Header::translateType(4, OFPT_PORT_MOD, 1));
  EXPECT_EQ(UInt8_cast(deprecated::v2::OFPT_PORT_MOD),
            Header::translateType(4, OFPT_PORT_MOD, 2));
  EXPECT_EQ(UInt8_cast(deprecated::v3::OFPT_PORT_MOD),
            Header::translateType(4, OFPT_PORT_MOD, 3));

  EXPECT_EQ(
      OFPT_PORT_MOD,
      Header::translateType(1, UInt8_cast(deprecated::v1::OFPT_PORT_MOD), 4));
  EXPECT_EQ(
      OFPT_PORT_MOD,
      Header::translateType(2, UInt8_cast(deprecated::v2::OFPT_PORT_MOD), 4));
  EXPECT_EQ(
      OFPT_PORT_MOD,
      Header::translateType(3, UInt8_cast(deprecated::v3::OFPT_PORT_MOD), 4));

  EXPECT_EQ(OFPT_UNSUPPORTED, Header::translateType(4, OFPT_SET_ASYNC, 1));
  EXPECT_EQ(OFPT_UNSUPPORTED, Header::translateType(4, OFPT_SET_ASYNC, 2));
  EXPECT_EQ(OFPT_UNSUPPORTED, Header::translateType(4, OFPT_SET_ASYNC, 3));
}


TEST(header, validateVersionAndType) 
{
    Header header{OFPT_UNSUPPORTED};

    header.setVersion(OFP_VERSION_1);
    for (unsigned type = 1; type <= OFPT_BARRIER_REPLY; ++type) {
        header.setType(static_cast<OFPType>(type));
        if (type == OFPT_GROUP_MOD || type == OFPT_TABLE_MOD) {
            EXPECT_FALSE(header.validateVersionAndType());
        } else {
            EXPECT_TRUE(header.validateVersionAndType());
        }
    }

    header.setVersion(OFP_VERSION_4);
    for (unsigned type = 1; type <= OFPT_METER_MOD; ++type) {
        header.setType(static_cast<OFPType>(type));
        EXPECT_TRUE(header.validateVersionAndType());
    }

    // Hello and Error messages validate when version is 0 (unset).
    header.setVersion(0);
    header.setType(OFPT_HELLO);
    EXPECT_TRUE(header.validateVersionAndType());
    header.setType(OFPT_ERROR);
    EXPECT_TRUE(header.validateVersionAndType());

    // All other messages do not validate when version is 0.
    for (unsigned type = OFPT_ERROR+1; type <= 255; ++type) {
        header.setType(static_cast<OFPType>(type));
        EXPECT_FALSE(header.validateVersionAndType());
    }
}
