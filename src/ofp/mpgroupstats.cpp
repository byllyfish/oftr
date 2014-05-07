#include "ofp/mpgroupstats.h"
#include "ofp/writable.h"

using namespace ofp;

PacketCounterRange MPGroupStats::bucketStats() const {
    assert(length_ >= sizeof(MPGroupStats));
    return ByteRange{BytePtr(this) + sizeof(MPGroupStats), length_ - sizeof(MPGroupStats)};
}

void MPGroupStatsBuilder::write(Writable *channel) {
    msg_.length_ = UInt16_narrow_cast(sizeof(msg_) + bucketStats_.size());

    channel->write(&msg_, sizeof(msg_));
    channel->write(bucketStats_.data(), bucketStats_.size());
    channel->flush();
}
