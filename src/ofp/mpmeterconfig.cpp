#include "ofp/mpmeterconfig.h"
#include "ofp/writable.h"

using namespace ofp;


MeterBandRange MPMeterConfig::bands() const {
    assert(length_ >= sizeof(MPMeterConfig));
    return ByteRange{BytePtr(this) + sizeof(MPMeterConfig), length_ - sizeof(MPMeterConfig)};
}


void MPMeterConfigBuilder::write(Writable *channel) {
    msg_.length_ = UInt16_narrow_cast(sizeof(MPMeterConfig) + bands_.size());

    channel->write(&msg_, sizeof(msg_));
    channel->write(bands_.data(), bands_.size());
    channel->flush();
}
