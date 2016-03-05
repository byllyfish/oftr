// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/setasync.h"
#include "ofp/asyncconfigproperty.h"
#include "ofp/unittest.h"

using namespace ofp;

const OFPPacketInFlags kPacketInFlags =
    static_cast<OFPPacketInFlags>(0x11111112);
const OFPPortStatusFlags kPortStatusFlags =
    static_cast<OFPPortStatusFlags>(0x22222221);
const OFPFlowRemovedFlags kFlowRemovedFlags =
    static_cast<OFPFlowRemovedFlags>(0x33333331);

TEST(setasync, builder) {
  PropertyList props;
  props.add(AsyncConfigPropertyPacketInSlave{kPacketInFlags});
  props.add(AsyncConfigPropertyPacketInMaster{kPacketInFlags});
  props.add(AsyncConfigPropertyPortStatusSlave{kPortStatusFlags});
  props.add(AsyncConfigPropertyPortStatusMaster{kPortStatusFlags});
  props.add(AsyncConfigPropertyFlowRemovedSlave{kFlowRemovedFlags});
  props.add(AsyncConfigPropertyFlowRemovedMaster{kFlowRemovedFlags});

  SetAsyncBuilder builder;
  builder.setProperties(props);

  {
    MemoryChannel channel{OFP_VERSION_5};
    builder.send(&channel);

    EXPECT_HEX(
        "051C003800000001000000081111111200010008111111120002000822222221000300"
        "082222222100040008333333310005000833333331",
        channel.data(), channel.size());
  }

  {
    MemoryChannel channel{OFP_VERSION_4};
    builder.send(&channel);

    EXPECT_HEX(
        "041C002000000001111111121111111222222221222222213333333133333331",
        channel.data(), channel.size());
  }
}
