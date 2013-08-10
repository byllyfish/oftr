#include "ofp/tcp_connection_impl.h"
#include "ofp/tcp_server_impl.h"
#include "ofp/driver_impl.h"
#include "ofp/log.h"

/*
static SharedPtr create(TCP_Server *server)
    {
        return SharedPtr{new TCP_Connection{server}};
    }
*/

ofp::impl::TCP_Connection::TCP_Connection(TCP_Server *server,
                                          tcp::socket socket)
    : server_{server}, socket_{std::move(socket)}
{
    log::debug(__PRETTY_FUNCTION__, this);
}

ofp::impl::TCP_Connection::~TCP_Connection()
{
    log::debug(__PRETTY_FUNCTION__, this);
}

ofp::impl::Driver_Impl *ofp::impl::TCP_Connection::driver() const
{
    return server_->driver();
}

void ofp::impl::TCP_Connection::start()
{
    log::debug(__PRETTY_FUNCTION__);

    message_.resize(sizeof(Header));
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

    boost::asio::async_read(socket_, boost::asio::buffer(message_.mutableData(), sizeof(Header)),
                                [this, self](error_code err, size_t length) {
        if (!err) {
        	assert(length == sizeof(Header));

        	const Header *hdr = message_.header();
        	UInt16 msgLength = hdr->length();
        	if (msgLength >= sizeof(Header)) {
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

    message_.resize(msgLength);
    boost::asio::async_read(socket_, boost::asio::buffer(message_.mutableData() + sizeof(Header), msgLength - sizeof(Header)),
                                [this, self](error_code err, size_t length) {
        
        if (!err) {
        	assert(length == message_.size() - sizeof(Header));

            message_.setAuxiliaryID(auxiliaryID_);
        	postMessage(this, &message_);
        	asyncReadHeader();

        } else {
        	log::debug("asyncReadMessage err ", err);
        }
    });
}

ofp::impl::tcp::socket &ofp::impl::TCP_Connection::socket()
{
    return socket_;
}

