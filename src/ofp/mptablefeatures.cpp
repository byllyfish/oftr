#include "ofp/mptablefeatures.h"
#include "ofp/writable.h"

using namespace ofp;


PropertyRange MPTableFeatures::properties() const {
    assert(length_ >= sizeof(MPTableFeatures));
    return ByteRange{BytePtr(this) + sizeof(MPTableFeatures), length_ - sizeof(MPTableFeatures)};
}


bool MPTableFeatures::validateInput(size_t length) const {
    if (length < sizeof(MPTableFeatures)) {
        return false;
    }

    size_t len = length_;
    if (len < sizeof(MPTableFeatures)) {
        return false;
    }

    if (len > length) {
        return false;
    }

    PropertyRange props = properties();
    if (!props.validateInput("MPTableFeatures")) {
        return false;
    }

    if (!TableFeaturePropertyValidator::validateInput(props)) {
        return false;
    }

    return true;
}

void MPTableFeaturesBuilder::write(Writable *channel) {
    msg_.length_ = UInt16_narrow_cast(sizeof(msg_) + properties_.size());
    channel->write(&msg_, sizeof(msg_));
    channel->write(properties_.data(), properties_.size());
    channel->flush();
}
