#ifndef OFP_SYS_TCP_CONNECTION_H
#define OFP_SYS_TCP_CONNECTION_H

#include "ofp/types.h"
#include "ofp/sys/boost_asio.h"
#include "ofp/sys/engine.h"
#include "ofp/message.h"
#include "ofp/sys/connection.h"
#include "ofp/driver.h"
#include "ofp/deferred.h"
#include "ofp/exception.h"

namespace ofp {  // <namespace ofp>
namespace sys { // <namespace sys>

class TCP_Server;

OFP_BEGIN_IGNORE_PADDING
class TCP_Connection : public std::enable_shared_from_this<TCP_Connection>, public Connection {
public:
	 TCP_Connection(Engine *engine, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory);
     TCP_Connection(Engine *engine, tcp::socket socket, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory);
     TCP_Connection(Engine *engine, DefaultHandshake *handshake);
    ~TCP_Connection();

    Deferred<Exception> asyncConnect(const tcp::endpoint &endpt, milliseconds delay = 0_ms);
    void asyncAccept();

    void channelUp();
    void channelException(const Exception &exc);
    void channelDown();

    bool isOutgoing() const { return endpoint_.port() != 0; }
    bool wantsReconnect() const { return handshake()->role() == Driver::Agent && isOutgoing(); }

    tcp::endpoint endpoint() const { return endpoint_; }
    
	IPv6Address remoteAddress() const override {
		try {
			if (isOutgoing()) {
				return makeIPv6Address(endpoint_.address());
			}
			return makeIPv6Address(socket_.remote_endpoint().address());
		} catch (std::exception &ex) {
			log::debug("remoteAddress:", ex.what());
			return IPv6Address{};
		}
	}

	UInt16 remotePort() const override {
		try {
			if (isOutgoing()) {
				return endpoint_.port();
			}
			return socket_.remote_endpoint().port();
		} catch (std::exception &ex) {
			log::debug("remotePort:", ex.what());
			return 0;
		}
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

	void shutdown() override {
		log::debug(__PRETTY_FUNCTION__);

		// FIXME -- this should stop reading data and close connection when 
		// all outgoing data existing at this point is sent.
		socket_.close();
	}

	Transport transport() const { return Transport::TCP; }

	void openAuxChannel(UInt8 auxID, Transport transport) override {
		engine()->openAuxChannel(auxID, transport, this);
	}

	Channel *findAuxChannel(UInt8 auxID) const override { return nullptr; }

private:
    Message message_;
    tcp::socket socket_;
    tcp::endpoint endpoint_;
	DeferredResultPtr<Exception> deferredExc_ = nullptr;
	steady_timer idleTimer_;
	std::chrono::steady_clock::time_point latestActivity_;

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
    void asyncConnect();
    void asyncDelayConnect(milliseconds delay);
    void asyncIdleCheck();

    void reconnect();
    void updateLatestActivity();
};
OFP_END_IGNORE_PADDING

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_TCP_CONNECTION_H
