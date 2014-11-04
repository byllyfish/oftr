// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/meterbandlist.h"
#include "ofp/meterbands.h"

using namespace ofp;

TEST(meterbandlist, test) {
    MeterBandList meterBands;

    meterBands.add(MeterBandDrop{0x11111111, 0x22222222});
    meterBands.add(MeterBandDscpRemark{0x33333333, 0x44444444, 0x55});
    //meterBands.add(MeterBandExperimenter{0x66666666, })
    
    EXPECT_EQ(32, meterBands.size());
    EXPECT_HEX("0001001011111111222222220000000000020010333333334444444455000000", meterBands.data(), meterBands.size());

    //MeterBandRange range = meterBands;

}
