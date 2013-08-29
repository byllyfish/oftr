#include "ofp/sys/connection.h"
#include "ofp/sys/engine.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>


Driver *Connection::driver() const
{
    return engine_->driver();
}

Connection::~Connection()
{
    if (listener_ != handshake_) {
        ChannelListener::dispose(listener_);
    }
    ChannelListener::dispose(handshake_);

    if (dpidWasPosted_) {
        engine()->releaseDatapathID(this);
    }
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


void Connection::postTimerExpired(ConnectionTimer *timer)
{
    log::debug("postTimerExpired");

    if (listener_) {
        listener_->onTimer(timer->id());
    }

    if (!timer->repeating()) {
        // Delete timer.
        size_t n = timers_.erase(timer->id());
        assert(n == 1);
    }
}

void Connection::postIdle()
{
    log::debug("postIdle() ==========");
}


void Connection::postDatapathID()
{
    assert(!dpidWasPosted_);

    engine()->postDatapathID(this);
    dpidWasPosted_ = true;
}


/// \brief Schedule a timer event on the channel and give it the specified ID.
/// If there is already a timer with the same ID, this method will cancel the 
/// old timer and replace it.
void Connection::scheduleTimer(UInt32 timerID, milliseconds interval, bool repeat) 
{
    timers_[timerID] = std::unique_ptr<ConnectionTimer>(new ConnectionTimer{this, timerID, interval, repeat});
}

void Connection::cancelTimer(UInt32 timerID) {
    size_t n = timers_.erase(timerID);
    assert(n == 1);
}

} // </namespace sys>
} // </namespace ofp>