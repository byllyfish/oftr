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

    if (dpidWasPosted_ && engine()->isRunning()) {
        engine()->releaseDatapathID(this);

        if (mainConn_ == this) {
            // Main connection is going down. Terminate all of our auxiliary 
            // connections.
            for (auto conn : auxList_) {
                conn->mainConn_ = conn;
                conn->shutdown();
            }

        } else {
            assert(mainConn_ != this);

            // Remove ourselves from the auxiliary connection list of our main
            // connection.
            assert(mainConn_ != nullptr);
            AuxiliaryList &auxList = mainConn_->auxList_;
            auto iter = std::find(auxList.begin(), auxList.end(), this);
            if (iter != auxList.end()) {
                auxList.erase(iter);
            } else {
                log::debug("~Connection: Missing pointer to auxiliary connection.");
            }
        }
    }
}

void Connection::setMainConnection(Connection *channel)
{
    assert(channel != nullptr);
    assert(mainConn_ == this);
    assert(channel != this);
    assert(auxiliaryID() != 0);

    log::debug("setMainConnection");
    mainConn_ = channel;

    UInt8 auxID = auxiliaryID();
    AuxiliaryList &auxList = mainConn_->auxList_;

    // Check if there is already an auxiliary connection with the same ID.
    // If so, close it so we can replace it with this one.

    auto iter = std::find_if(auxList.begin(), auxList.end(), [auxID](Connection *conn) {
        return conn->auxiliaryID() == auxID;
    });
    if (iter != auxList.end()) {
        log::info("setMainConnection: Auxiliary connection found with same ID.");
        (*iter)->shutdown();
    }

    auxList.push_back(this);
}

void Connection::postMessage(Connection *source, Message *message)
{
    log::debug("Read: ",
               RawDataToHex(message->data(), message->size(), ' ', 2));
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
void Connection::scheduleTimer(UInt32 timerID, milliseconds interval,
                               bool repeat)
{
    timers_[timerID] = std::unique_ptr<ConnectionTimer>(
        new ConnectionTimer{this, timerID, interval, repeat});
}

void Connection::cancelTimer(UInt32 timerID)
{
    size_t n = timers_.erase(timerID);
    assert(n == 1);
}

} // </namespace sys>
} // </namespace ofp>