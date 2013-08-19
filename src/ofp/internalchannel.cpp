#include "ofp/internalchannel.h"
#include "ofp/impl/engine.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

Driver *InternalChannel::driver() const
{
    return engine_->driver();
}

InternalChannel::~InternalChannel()
{
    ChannelListener::dispose(listener_);
}

UInt8 InternalChannel::version() const
{
    return version_;
}

void InternalChannel::setVersion(UInt8 version)
{
    version_ = version;
}

const Features &InternalChannel::features() const
{
    return features_;
}

void InternalChannel::setFeatures(const Features &features)
{
    features_ = features;
}

void InternalChannel::postMessage(InternalChannel *source, Message *message)
{
    log::debug("Read: ", RawDataToHex(message->data(), message->size(), ' ', 2));
    if (listener_) {
        listener_->onMessage(message);
    }
}

} // </namespace ofp>