// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/constants_error.h"

using namespace ofp;

TEST(constants_error, test) {
  EXPECT_EQ(OFPFMFC_UNSUPPORTED, OFPErrorCodeFromValue(OFP_VERSION_1, 3, 5));
  EXPECT_EQ(OFPBIC_BAD_EXPERIMENTER,
            OFPErrorCodeFromValue(OFP_VERSION_4, 3, 5));

  EXPECT_EQ(3, OFPErrorTypeToValue(OFP_VERSION_1, OFPFMFC_UNSUPPORTED));
  EXPECT_EQ(5, OFPErrorCodeToValue(OFP_VERSION_1, OFPFMFC_UNSUPPORTED));

  EXPECT_EQ(3, OFPErrorTypeToValue(OFP_VERSION_4, OFPBIC_BAD_EXPERIMENTER));
  EXPECT_EQ(5, OFPErrorCodeToValue(OFP_VERSION_4, OFPBIC_BAD_EXPERIMENTER));
}
