#include "ofp/unittest.h"
#include "ofp/setasync.h"

using namespace ofp;

const OFPPacketInFlags kPacketInFlags = static_cast<OFPPacketInFlags>(0x11111112);
const OFPPortStatusFlags kPortStatusFlags = static_cast<OFPPortStatusFlags>(0x22222221);
const OFPFlowRemovedFlags kFlowRemovedFlags = static_cast<OFPFlowRemovedFlags>(0x33333331);


TEST(setasync, builder) {
    /*
    PropertyList props;
    props.add(AsyncConfigPropertyPacketInSlave{0});
    props.add(AsyncConfigPropertyPacketInMaster{1});
    props.add(AsyncConfigPropertyPortStatusSlave{0});
    props.add(AsyncConfigPropertyPortStatusMaster{1});
    props.add(AsyncConfigPropertyFlowRemovedSlave{0});
    props.add(AsyncConfigPropertyFlowRemovedMaster{1});
*/

    SetAsyncBuilder builder;
    builder.setMasterPacketInMask(kPacketInFlags);
    builder.setSlavePacketInMask(kPacketInFlags);
    builder.setMasterPortStatusMask(kPortStatusFlags);
    builder.setSlavePortStatusMask(kPortStatusFlags);
    builder.setMasterFlowRemovedMask(kFlowRemovedFlags);
    builder.setSlaveFlowRemovedMask(kFlowRemovedFlags);

    //builder.setProperties(props);

    MemoryChannel channel{OFP_VERSION_4};
    builder.send(&channel);

    EXPECT_HEX("041C002000000001111111121111111222222221222222213333333133333331", channel.data(), channel.size());

    Message message(channel.data(), channel.size());
    message.transmogrify();

    const SetAsync *msg = SetAsync::cast(&message);
    ASSERT_NE(nullptr, msg);

    EXPECT_EQ(kPacketInFlags, msg->masterPacketInMask());
    EXPECT_EQ(kPacketInFlags, msg->slavePacketInMask());
    EXPECT_EQ(kPortStatusFlags, msg->masterPortStatusMask());
    EXPECT_EQ(kPortStatusFlags, msg->slavePortStatusMask());
    EXPECT_EQ(kFlowRemovedFlags, msg->masterFlowRemovedMask());
    EXPECT_EQ(kFlowRemovedFlags, msg->slaveFlowRemovedMask());
}
