#ifndef OFP_IMPL_TCP_CONNECTION_H
#define OFP_IMPL_TCP_CONNECTION_H

#include "ofp/types.h"
#include "ofp/impl/boost_asio.h"
#include "ofp/impl/engine.h"
#include "ofp/message.h"
#include "ofp/connection.h"
#include "ofp/driver.h"
#include "ofp/deferred.h"
#include "ofp/exception.h"

namespace ofp {  // <namespace ofp>
namespace impl { // <namespace impl>

class TCP_Server;

OFP_BEGIN_IGNORE_PADDING
class TCP_Connection : public std::enable_shared_from_this<TCP_Connection>, public Connection {
public:
	 TCP_Connection(Engine *engine, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory);
     TCP_Connection(Engine *engine, tcp::socket socket, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory);
    ~TCP_Connection();

    Deferred<Exception> asyncConnect(const tcp::endpoint &endpt);
    void asyncAccept();

    void channelUp();
    void channelException(const Exception &exc);
    void channelDown();

	IPv6Address remoteAddress() const override {
		return makeIPv6Address(socket_.remote_endpoint().address());
	}

	UInt16 remotePort() const override {
		return socket_.remote_endpoint().port();
	}
	

	void write(const void *data, size_t length) override 
	{
		outgoing_[outgoingIdx_].add(data, length);
	}

	void flush() override
	{
		if (!writing_) {
			asyncWrite();
		}
	}

	void close() override {
		log::debug(__PRETTY_FUNCTION__);
		socket_.close();
	}

	Transport transport() const { return Transport::TCP; }

	void openAuxChannel(UInt8 auxID, Transport transport) override {
		//TODO   engine()->openAuxChannel(this, endpt);
	}

	Channel *findAuxChannel(UInt8 auxID) const override { return nullptr; }

private:
    Message message_;
    tcp::socket socket_;
    //Features features_{};
	//UInt32 nextXid_ = 0;
	//UInt8 protocolVersion_ = 0;
	//ChannelListener *listener_ = nullptr;
	DeferredResultPtr<Exception> deferredExc_ = nullptr;

	// Use a two buffer strategy for async-writes. We queue up data in one
	// buffer while we're in the process of writing the other buffer.
	ByteList outgoing_[2];
	int outgoingIdx_ = 0;
	bool writing_ = false;
	std::function<void()> flushCallback_ = nullptr;

	log::Lifetime lifetime_{"TCP_Connection"};

    void asyncReadHeader();
    void asyncReadMessage(size_t length);
    void asyncWrite();
    void asyncEchoReply(const Header *header, size_t length);
    void asyncRelay(size_t length);
};
OFP_END_IGNORE_PADDING

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_IMPL_TCP_CONNECTION_H
