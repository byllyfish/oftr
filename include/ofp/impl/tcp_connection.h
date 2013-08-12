#ifndef OFP_IMPL_TCP_CONNECTION_H
#define OFP_IMPL_TCP_CONNECTION_H

#include "ofp/types.h"
#include "ofp/impl/boost_asio.h"
#include "ofp/impl/engine.h"
#include "ofp/message.h"
#include "ofp/internalchannel.h"
#include "ofp/features.h"
#include "ofp/driver.h"
#include "ofp/deferred.h"
#include "ofp/exception.h"

namespace ofp {  // <namespace ofp>
namespace impl { // <namespace impl>

class TCP_Server;

class TCP_Connection : public std::enable_shared_from_this<TCP_Connection>, public InternalChannel {
public:
	 TCP_Connection(Engine *engine, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory);
     TCP_Connection(Engine *engine, tcp::socket socket, Driver::Role role, ProtocolVersions versions, ChannelListener::Factory factory);
    ~TCP_Connection();

    Deferred<Exception> asyncConnect(const tcp::endpoint &endpt);
    void asyncAccept();

    void channelUp();
    //void stop();

	const Features *features() const override {
		return &features_;
	}

	void setFeatures(const Features &features) override {
		features_ = features;
	}

	UInt8 protocolVersion() const override {
		return protocolVersion_;
	}

	void setProtocolVersion(UInt8 version) override {
		log::debug("protocol version ", int(version));
		protocolVersion_ = version;
	}

	UInt32 nextXid() override {
		return ++nextXid_;
	}

	UInt8 auxiliaryID() const override {
		return features_.auxiliaryID();
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

	//void sendError(UInt16 type, UInt16 code) override 
	//{
//
	//}

	void close() override {

	}

	ChannelListener *setChannelListener(ChannelListener *listener) override {
		ChannelListener *old = listener_;
		listener_ = listener;
		return old;
	}

	void postMessage(InternalChannel *source, Message *message) override {
		if (listener_)
			listener_->onMessage(message);
	}

	InternalChannel *mainConnection() override { return nullptr; }
	void setMainConnection(InternalChannel *channel) override {}

	InternalChannel *nextAuxiliaryConnection() override { return nullptr;}
	void setNextAuxiliaryConnection(InternalChannel *channel) override {}

    tcp::socket &socket();
    ///Engine *driver() const;

private:
    Engine *engine_;
    Message message_;
    tcp::socket socket_;
    Features features_{};
	UInt32 nextXid_ = 0;
	UInt8 protocolVersion_ = 0;
	ChannelListener *listener_ = nullptr;
	DeferredResultPtr<Exception> deferredExc_ = nullptr;

	// Use a two buffer strategy for async-writes. We queue up data in one
	// buffer while we're in the process of writing the other buffer.
	ByteList outgoing_[2];
	int outgoingIdx_ = 0;
	bool writing_ = false;

	//UInt8 auxiliaryID_ = 0;

    void asyncReadHeader();
    void asyncReadMessage(size_t length);

    void asyncWrite();
};

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_IMPL_TCP_CONNECTION_H
