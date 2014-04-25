#include "ofp/mptablefeatures.h"
#include "ofp/writable.h"

using namespace ofp;

void MPTableFeaturesBuilder::write(Writable *channel) {
    msg_.length_ = sizeof(msg_) + properties_.size();
    channel->write(&msg_, sizeof(msg_));
    channel->write(properties_.data(), properties_.size());
    channel->flush();
}
