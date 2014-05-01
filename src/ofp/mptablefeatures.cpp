#include "ofp/mptablefeatures.h"
#include "ofp/writable.h"

using namespace ofp;


PropertyRange MPTableFeatures::properties() const {
    assert(length_ >= sizeof(MPTableFeatures));
    return ByteRange{BytePtr(this) + sizeof(MPTableFeatures), length_ - sizeof(MPTableFeatures)};
}

void MPTableFeaturesBuilder::write(Writable *channel) {
    msg_.length_ = UInt16_narrow_cast(sizeof(msg_) + properties_.size());
    channel->write(&msg_, sizeof(msg_));
    channel->write(properties_.data(), properties_.size());
    channel->flush();
}
