#include "ofp/impl/tcp_connection.h"
#include "ofp/impl/tcp_server.h"
#include "ofp/impl/engine.h"
#include "ofp/log.h"
#include "ofp/defaulthandshake.h"

using namespace boost;


ofp::impl::TCP_Connection::TCP_Connection(impl::Engine *engine, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory)
    : InternalChannel{engine, new DefaultHandshake{this, role, versions, factory}}, message_{this}, socket_{engine->io()}
{
    log::debug(__PRETTY_FUNCTION__, this);
}

ofp::impl::TCP_Connection::TCP_Connection(Engine *engine, tcp::socket socket, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory)
    : InternalChannel{engine, new DefaultHandshake{this, role, versions, factory}}, message_{this}, socket_{std::move(socket)}
{
    log::debug(__PRETTY_FUNCTION__, this);
}

ofp::impl::TCP_Connection::~TCP_Connection()
{
    log::debug(__PRETTY_FUNCTION__, this);
}


ofp::Deferred<ofp::Exception> ofp::impl::TCP_Connection::asyncConnect(const tcp::endpoint &endpt)
{
    log::debug(__PRETTY_FUNCTION__);

    assert(deferredExc_ == nullptr);

    deferredExc_ = Deferred<Exception>::makeResult();

    auto self(shared_from_this());
    socket_.async_connect(endpt, [this, self](const error_code &err) {

        log::debug("async_connect ", err);

        deferredExc_->done(makeException(err));
        deferredExc_ = nullptr;

        if (!err) {
            assert(socket_.is_open());
            channelUp();
        }
    });

    return deferredExc_;
}


void ofp::impl::TCP_Connection::asyncAccept()
{
    channelUp();
}


void ofp::impl::TCP_Connection::channelUp()
{
    log::debug(__PRETTY_FUNCTION__);

    channelListener()->onChannelUp(this);
    asyncReadHeader();
}


void ofp::impl::TCP_Connection::channelException(const Exception &exc)
{
    channelListener()->onException(&exc);
}

void ofp::impl::TCP_Connection::channelDown()
{
    channelListener()->onChannelDown(this);
    close();
}


/**
void ofp::impl::TCP_Connection::stop()
{
	log::debug(__PRETTY_FUNCTION__);

	socket_.close();
}
**/

void ofp::impl::TCP_Connection::asyncReadHeader()
{
    log::debug(__PRETTY_FUNCTION__);

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

        		asyncReadMessage(msgLength);
                
        	} else {
        		log::debug("Message too short.");
        	}
        } else {
        	

            if (!isEOF(err)) {
                auto exc = makeException(err);
                log::debug("asyncReadHeader err ", exc);
                channelException(makeException(err));   
            }
            channelDown();
        }
    });
}

void ofp::impl::TCP_Connection::asyncReadMessage(size_t msgLength)
{
    log::debug(__PRETTY_FUNCTION__, msgLength);

    auto self(shared_from_this());

    asio::async_read(socket_, asio::buffer(message_.mutableData(msgLength) + sizeof(Header), msgLength - sizeof(Header)),
                                [this, self](const error_code &err, size_t bytes_transferred) {
        
        if (!err) {
        	assert(bytes_transferred == message_.size() - sizeof(Header));

            log::debug("Read: ", RawDataToHex(message_.data(), message_.size(), ' ', 2));
        	postMessage(this, &message_);
        	asyncReadHeader();

        } else {
            if (!isEOF(err)) {
                auto exc = makeException(err);
                log::info("asyncReadMessage err ", exc);
                channelException(makeException(err));   
            }
            channelDown();
        }
    });
}


void ofp::impl::TCP_Connection::asyncWrite()
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

    ::asio::async_write(socket_, asio::buffer(data, size), [this, self](const error_code &err, size_t bytes_transferred) {

        if (!err) {
            assert(bytes_transferred == outgoing_[!outgoingIdx_].size());

            writing_ = false;
            outgoing_[!outgoingIdx_].clear();
            if (outgoing_[outgoingIdx_].size() > 0) {
                asyncWrite();
            }
        } else {
            log::debug("Write error ", makeException(err));
        }
    });
}

