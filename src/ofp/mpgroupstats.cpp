#include "ofp/mpgroupstats.h"
#include "ofp/writable.h"
#include "ofp/validation.h"

using namespace ofp;

PacketCounterRange MPGroupStats::bucketStats() const {
  assert(length_ >= sizeof(MPGroupStats));
  return ByteRange{BytePtr(this) + sizeof(MPGroupStats),
                   length_ - sizeof(MPGroupStats)};
}

bool MPGroupStats::validateInput(Validation *context) const {
  if (!context->validateLength(length_, sizeof(MPGroupStats))) {
    return false;
  }

  if (!bucketStats().validateInput(context)) {
    return false;
  }

  return true;
}

void MPGroupStatsBuilder::write(Writable *channel) {
  msg_.length_ = UInt16_narrow_cast(sizeof(msg_) + bucketStats_.size());

  channel->write(&msg_, sizeof(msg_));
  channel->write(bucketStats_.data(), bucketStats_.size());
  channel->flush();
}
