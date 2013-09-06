#include "ofp/sys/tcp_connection.h"
#include "ofp/sys/tcp_server.h"
#include "ofp/sys/engine.h"
#include "ofp/log.h"
#include "ofp/defaulthandshake.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

using namespace boost;


TCP_Connection::TCP_Connection(Engine *engine, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, role, versions, factory}}, message_{this}, socket_{engine->io()}, idleTimer_{engine->io()}
{
}

TCP_Connection::TCP_Connection(Engine *engine, tcp::socket socket, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, role, versions, factory}}, message_{this}, socket_{std::move(socket)}, idleTimer_{engine->io()}
{
}

/// \brief Construct connection object for reconnect attempt.
TCP_Connection::TCP_Connection(Engine *engine, DefaultHandshake *handshake)
: Connection{engine, handshake}, message_{this}, socket_{engine->io()}, idleTimer_{engine->io()}
{
    handshake->setConnection(this);
}

TCP_Connection::~TCP_Connection()
{
}


Deferred<ofp::Exception> TCP_Connection::asyncConnect(const tcp::endpoint &endpt, milliseconds delay)
{
    assert(deferredExc_ == nullptr);

    endpoint_ = endpt;
    deferredExc_ = Deferred<Exception>::makeResult();

    if (delay > 0_ms) {
        asyncDelayConnect(delay);

    } else {
        asyncConnect();
    }

    return deferredExc_;
}


void TCP_Connection::asyncAccept()
{
    channelUp();
}


void TCP_Connection::channelUp()
{
    assert(channelListener());

    channelListener()->onChannelUp(this);
    asyncReadHeader();

    updateLatestActivity();
    asyncIdleCheck();
}


void TCP_Connection::channelException(const Exception &exc)
{
    assert(channelListener());

    channelListener()->onException(&exc);
}

void TCP_Connection::channelDown()
{
    assert(channelListener());

    channelListener()->onChannelDown(this);

    if (wantsReconnect()) {
        reconnect();
    }
}


void TCP_Connection::asyncReadHeader()
{
    // Do nothing if socket is not open.
    if (!socket_.is_open())
        return;

    auto self(shared_from_this());

    asio::async_read(socket_, asio::buffer(message_.mutableData(sizeof(Header)), sizeof(Header)),
                                [this, self](error_code err, size_t length) {

        if (!err) {
        	assert(length == sizeof(Header));

        	const Header *hdr = message_.header();

            // If version has already been negotiated, check version in the 
            // header. {TODO} Then check the length and type. Each message type
            // should have a limit to its length.

        	UInt16 msgLength = hdr->length();
        	if (msgLength >= sizeof(Header)) {
                // TODO once we have the header, we can check the version, type,
                // and length fields.

                if (hdr->type() == OFPT_ECHO_REQUEST) {
                    // Handle echo request by relaying message back as we read 
                    // it in. In this way, we don't have to read the whole 
                    // message in before we start to respond.
                    asyncEchoReply(hdr, msgLength);

                } else if (msgLength == sizeof(Header)) {
                    postMessage(this, &message_);
                    asyncReadHeader();

                } else {
                    asyncReadMessage(msgLength);
                }
                
        	} else {
        		log::debug("Message too short.");
        	}
        } else {
        	
            if (!isAsioEOF(err)) {
                auto exc = makeException(err);
                log::debug("asyncReadHeader err ", exc);
                channelException(makeException(err));   
            }

            channelDown();
        }

        updateLatestActivity();
    });
}

void TCP_Connection::asyncReadMessage(size_t msgLength)
{
    assert(msgLength > sizeof(Header));

    auto self(shared_from_this());

    asio::async_read(socket_, asio::buffer(message_.mutableData(msgLength) + sizeof(Header), msgLength - sizeof(Header)),
                                [this, self](const error_code &err, size_t bytes_transferred) {

        if (!err) {
        	assert(bytes_transferred == message_.size() - sizeof(Header));

        	postMessage(this, &message_);
        	asyncReadHeader();

        } else {
            if (!isAsioEOF(err)) {
                auto exc = makeException(err);
                log::info("asyncReadMessage err ", exc);
                channelException(makeException(err));   
            }
            channelDown();
        }

        updateLatestActivity();
    });
}


void TCP_Connection::asyncWrite()
{
    assert(!writing_);

    int idx = outgoingIdx_;
    outgoingIdx_ = !outgoingIdx_;
    writing_ = true;

    const UInt8 *data = outgoing_[idx].data();
    size_t size = outgoing_[idx].size();

    log::trace("asyncWrite", data, size);

    auto self(shared_from_this());

    asio::async_write(socket_, asio::buffer(data, size), [this, self](const error_code &err, size_t bytes_transferred) {

        if (!err) {
            assert(bytes_transferred == outgoing_[!outgoingIdx_].size());

            writing_ = false;
            outgoing_[!outgoingIdx_].clear();
            if (outgoing_[outgoingIdx_].size() > 0) {
                // Start another async write for the other output buffer.
                asyncWrite();
            } else if (flushCallback_ != nullptr) {
                // Outgoing buffer is empty and we want to write direct.
                flushCallback_();
                flushCallback_ = nullptr;
            }

        } else {
            log::debug("Write error ", makeException(err));
        }

        updateLatestActivity();
    });
}

void TCP_Connection::asyncEchoReply(const Header *header, size_t length)
{
    assert(length >= sizeof(Header));

    log::debug("asyncEchoReply");

    Header replyHeader = *header;
    replyHeader.setType(OFPT_ECHO_REPLY);
    write(&replyHeader, sizeof(replyHeader));
    flush();

    length -= sizeof(Header);

    if (length > 0) {
        assert(writing_);
        assert(flushCallback_ == nullptr);

        auto self(shared_from_this());
        flushCallback_ = [this, self, length]() {
            // Immediately write back the next `length` bytes we receive.
            // When we're done, go back to asyncReadHeader.
            asyncRelay(length);
        };

    } else {
        asyncReadHeader();
    }
}

void TCP_Connection::asyncRelay(size_t length)
{
    if (length > 0) {
        auto self(shared_from_this());

        size_t len = std::min(length, message_.size());
        socket_.async_read_some(asio::buffer(message_.mutableData(128), len), 
            [this, self, length](const error_code &readErr, size_t bytesRead) {

            if (!readErr) {
                asio::async_write(socket_, asio::buffer(message_.data(), bytesRead),
                    [this, self, length](const error_code &writeErr, size_t bytesWritten) {

                    if (!writeErr) {
                        assert(bytesWritten <= length);
                        asyncRelay(length - bytesWritten);
                    } else {
                        log::debug("asyncRelay - async_write error", makeException(writeErr));
                    }
                });

            } else {
                log::debug("asyncRelay - async_read_some error", makeException(readErr));
            }
        });

    } else {
        asyncReadHeader();
    }
}


void TCP_Connection::asyncConnect()
{
    auto self(shared_from_this());

    socket_.async_connect(endpoint_, [this, self](const error_code &err) {
        // `async_connect` may not report an error when the connection attempt
        // fails. We need to double-check that we are connected.

        error_code actualErr = err;

        if (!actualErr && checkAsioConnected(socket_, endpoint_, actualErr)) {
            channelUp();

        } else if (wantsReconnect()) {
            reconnect();
        }

        deferredExc_->done(makeException(actualErr));
        deferredExc_ = nullptr;
    });
}


void TCP_Connection::asyncDelayConnect(milliseconds delay)
{
    auto self(shared_from_this());

    idleTimer_.expires_from_now(delay);
    idleTimer_.async_wait([this, self](const error_code &err) {
        if (err != boost::asio::error::operation_aborted) {
            asyncConnect();
        } else {
            assert(deferredExc_ != nullptr);
            deferredExc_->done(makeException(err));
            deferredExc_ = nullptr;
        }
    });
}

void TCP_Connection::asyncIdleCheck()
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    auto interval = std::chrono::duration_cast<milliseconds>(now - latestActivity_);

    milliseconds delay;
    if (interval >= 5000_ms) {
        postIdle();
        delay = 5000_ms;
    } else {
        delay = 5000_ms - interval;
    }

    idleTimer_.expires_from_now(delay);
    idleTimer_.async_wait([this](const error_code &err) {
        if (err != boost::asio::error::operation_aborted) {
            asyncIdleCheck();
        }
    });
}


void TCP_Connection::reconnect()
{
    DefaultHandshake *hs = handshake();
    assert(hs);

    hs->setStartingVersion(version());
    hs->setStartingXid(nextXid());

    log::debug("reconnecting...", remoteAddress());

    engine()->reconnect(hs, &features(), remoteAddress(), remotePort(), 750_ms);

    setHandshake(nullptr);
    if (channelListener() == hs) {
        setChannelListener(nullptr);
    }

    assert(channelListener() != hs);
}


void TCP_Connection::updateLatestActivity()
{
    latestActivity_ = std::chrono::steady_clock::now();
}


} // </namespace sys>
} // </namespace ofp>
