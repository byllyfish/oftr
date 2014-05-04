#include "ofp/mpgroupdesc.h"

using namespace ofp;

BucketRange MPGroupDesc::buckets() const {
    assert(length_ >= sizeof(MPGroupDesc));

    return ByteRange{BytePtr(this) + sizeof(MPGroupDesc), length_ - sizeof(MPGroupDesc)};
}


void MPGroupDescBuilder::write(Writable *channel)
{
    size_t msgLen = sizeof(msg_) + buckets_.size();

    msg_.length_ = UInt16_narrow_cast(msgLen);

    channel->write(&msg_, sizeof(msg_));
    channel->write(buckets_.data(), buckets_.size());
    channel->flush();
}