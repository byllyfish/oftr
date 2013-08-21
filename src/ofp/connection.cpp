#include "ofp/connection.h"
#include "ofp/impl/engine.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

Driver *Connection::driver() const
{
    return engine_->driver();
}

Connection::~Connection()
{
    ChannelListener::dispose(listener_);
}

UInt8 Connection::version() const
{
    return version_;
}

void Connection::setVersion(UInt8 version)
{
    version_ = version;
}

const Features &Connection::features() const
{
    return features_;
}

void Connection::setFeatures(const Features &features)
{
    features_ = features;
}

void Connection::postMessage(Connection *source, Message *message)
{
    log::debug("Read: ", RawDataToHex(message->data(), message->size(), ' ', 2));
    if (listener_) {
        listener_->onMessage(message);
    }
}

} // </namespace ofp>