#include "ofp/mpmeterstats.h"
#include "ofp/writable.h"

using namespace ofp;

PacketCounterRange MPMeterStats::bandStats() const {
    assert(len_ >= sizeof(MPMeterStats));
    return ByteRange{BytePtr(this) + sizeof(MPMeterStats), len_ - sizeof(MPMeterStats)};
}


void MPMeterStatsBuilder::write(Writable *channel) {
    msg_.len_ = UInt16_narrow_cast(sizeof(msg_) + bandStats_.size());
    channel->write(&msg_, sizeof(msg_));
    channel->write(bandStats_.data(), bandStats_.size());
    channel->flush();
}
