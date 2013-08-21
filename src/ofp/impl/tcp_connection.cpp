#include "ofp/impl/tcp_connection.h"
#include "ofp/impl/tcp_server.h"
#include "ofp/impl/engine.h"
#include "ofp/log.h"
#include "ofp/defaulthandshake.h"

namespace ofp { // <namespace ofp>
namespace impl { // <namespace impl>


using namespace boost;


TCP_Connection::TCP_Connection(Engine *engine, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, role, versions, factory}}, message_{this}, socket_{engine->io()}
{
    log::debug(__PRETTY_FUNCTION__, this);
}

TCP_Connection::TCP_Connection(Engine *engine, tcp::socket socket, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, role, versions, factory}}, message_{this}, socket_{std::move(socket)}
{
    log::debug(__PRETTY_FUNCTION__, this);
}

TCP_Connection::~TCP_Connection()
{
    //log::debug(__PRETTY_FUNCTION__, this);
}


Deferred<ofp::Exception> TCP_Connection::asyncConnect(const tcp::endpoint &endpt)
{
    log::debug(__PRETTY_FUNCTION__);

    assert(deferredExc_ == nullptr);

    deferredExc_ = Deferred<Exception>::makeResult();

    auto self(shared_from_this());
    socket_.async_connect(endpt, [this, self](const error_code &err) {
        log::Lifetime lifetime{"asyncConnect callback"};

        deferredExc_->done(makeException(err));
        deferredExc_ = nullptr;

        if (!err) {
            assert(socket_.is_open());
            channelUp();
        }
    });

    return deferredExc_;
}


void TCP_Connection::asyncAccept()
{
    channelUp();
}


void TCP_Connection::channelUp()
{
    channelListener()->onChannelUp(this);
    asyncReadHeader();
}


void TCP_Connection::channelException(const Exception &exc)
{
    channelListener()->onException(&exc);
}

void TCP_Connection::channelDown()
{
    channelListener()->onChannelDown(this);
}


/**
void ofp::impl::TCP_Connection::stop()
{
	log::debug(__PRETTY_FUNCTION__);

	socket_.close();
}
**/

void TCP_Connection::asyncReadHeader()
{
    log::debug(__PRETTY_FUNCTION__);

    auto self(shared_from_this());

    asio::async_read(socket_, asio::buffer(message_.mutableData(sizeof(Header)), sizeof(Header)),
                                [this, self](error_code err, size_t length) {
        log::Lifetime lifetime("asyncReadHeader callback");

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
    });
}

void TCP_Connection::asyncReadMessage(size_t msgLength)
{
    assert(msgLength > sizeof(Header));

    log::debug(__PRETTY_FUNCTION__, msgLength);

    auto self(shared_from_this());

    asio::async_read(socket_, asio::buffer(message_.mutableData(msgLength) + sizeof(Header), msgLength - sizeof(Header)),
                                [this, self](const error_code &err, size_t bytes_transferred) {
        log::Lifetime lifetime{"asyncReadMessage callback"};

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
    });
}


void TCP_Connection::asyncWrite()
{
    assert(!writing_);
    log::debug(__PRETTY_FUNCTION__);

    int idx = outgoingIdx_;
    outgoingIdx_ = !outgoingIdx_;
    writing_ = true;

    const UInt8 *data = outgoing_[idx].data();
    size_t size = outgoing_[idx].size();

    log::debug("Write: ", RawDataToHex(data, size, ' ', 2));

    auto self(shared_from_this());

    asio::async_write(socket_, asio::buffer(data, size), [this, self](const error_code &err, size_t bytes_transferred) {
        log::Lifetime lifetime{"asyncWrite callback"};

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
            log::Lifetime lifetime{"asyncEchoReply callback"};

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
    log::debug("asyncRelay:", length);

    if (length > 0) {
        auto self(shared_from_this());

        size_t len = std::min(length, message_.size());
        socket_.async_read_some(asio::buffer(message_.mutableData(128), len), 
            [this, self, length](const error_code &readErr, size_t bytesRead) {
            log::Lifetime lifetime{"async_read_some callback"};

            if (!readErr) {
                asio::async_write(socket_, asio::buffer(message_.data(), bytesRead),
                    [this, self, length](const error_code &writeErr, size_t bytesWritten) {
                    log::Lifetime lifetime{"async_write_some callback"};

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


} // </namespace impl>
} // </namespace ofp>
