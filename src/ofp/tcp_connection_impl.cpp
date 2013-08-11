#include "ofp/tcp_connection_impl.h"
#include "ofp/tcp_server_impl.h"
#include "ofp/driver_impl.h"
#include "ofp/log.h"
#include "ofp/controllerhandshake.h"

using namespace boost;

/*
static SharedPtr create(TCP_Server *server)
    {
        return SharedPtr{new TCP_Connection{server}};
    }
*/

ofp::impl::TCP_Connection::TCP_Connection(TCP_Server *server,
                                          tcp::socket socket, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory)
    : server_{server}, socket_{std::move(socket)}, message_{this}
{
    log::debug(__PRETTY_FUNCTION__, this);

    if (role == Driver::Controller) {
        listener_ = new ControllerHandshake{this, versions, factory};
    } else {
        // TODO
    }
}

ofp::impl::TCP_Connection::~TCP_Connection()
{
    log::debug(__PRETTY_FUNCTION__, this);

    ChannelListener::dispose(listener_);
}

ofp::impl::Driver_Impl *ofp::impl::TCP_Connection::driver() const
{
    return server_->driver();
}

void ofp::impl::TCP_Connection::start()
{
    log::debug(__PRETTY_FUNCTION__);

    if (listener_)
        listener_->onChannelUp(this);

    asyncReadHeader();
}

void ofp::impl::TCP_Connection::stop()
{
	log::debug(__PRETTY_FUNCTION__);

	socket_.close();
}

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
        	log::debug("asyncReadHeader err ", err);
        }
    });
}

void ofp::impl::TCP_Connection::asyncReadMessage(size_t msgLength)
{
    log::debug(__PRETTY_FUNCTION__, msgLength);

    auto self(shared_from_this());

    asio::async_read(socket_, asio::buffer(message_.mutableData(msgLength) + sizeof(Header), msgLength - sizeof(Header)),
                                [this, self](error_code err, size_t length) {
        
        if (!err) {
        	assert(length == message_.size() - sizeof(Header));

        	postMessage(this, &message_);
        	asyncReadHeader();

        } else {
        	log::debug("asyncReadMessage err ", err);
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
    ::asio::async_write(socket_, asio::buffer(data, size), [this, self](error_code err, size_t length) {
        writing_ = false;
        outgoing_[!outgoingIdx_].clear();
        if (outgoing_[outgoingIdx_].size() > 0) {
            asyncWrite();
        }
    });
}

ofp::impl::tcp::socket &ofp::impl::TCP_Connection::socket()
{
    return socket_;
}

