#ifndef OFP_TCP_CONNECTION_IMPL_H
#define OFP_TCP_CONNECTION_IMPL_H

#include "ofp/types.h"
#include "ofp/boost_impl.h"
#include "ofp/message.h"
#include "ofp/internalchannel_impl.h"

namespace ofp {  // <namespace ofp>
namespace impl { // <namespace impl>

class TCP_Server;
class Driver_Impl;

class TCP_Connection : public std::enable_shared_from_this<TCP_Connection>, public InternalChannel {
public:
     TCP_Connection(TCP_Server *server, tcp::socket socket);
    ~TCP_Connection();

    void start();
    void stop();

	DatapathID datapathID() const override {
		return dpid_;
	}

	UInt8 version() const override {
		return version_;
	}

	UInt32 nextXid() override {
		return ++nextXid_;
	}

	UInt8 auxiliaryID() const override {
		return auxiliaryID_;
	}

	void write(const void *data, size_t length) override {

	}

	void postMessage(InternalChannel *source, Message *message) override {
		
	}

	InternalChannel *mainConnection() override { return nullptr; }
	void setMainConnection(InternalChannel *channel) override {}

	InternalChannel *nextAuxiliaryConnection() override { return nullptr;}
	void setNextAuxiliaryConnection(InternalChannel *channel) override {}

    tcp::socket &socket();
    Driver_Impl *driver() const;

private:
    TCP_Server *server_;
    tcp::socket socket_;
    Message message_;
    DatapathID dpid_;
	UInt32 nextXid_ = 0;
	UInt8 version_ = 0;
	UInt8 auxiliaryID_ = 0;

    void asyncReadHeader();
    void asyncReadMessage(size_t length);
};

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_TCP_CONNECTION_IMPL_H
