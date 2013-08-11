#ifndef OFP_DEFAULT_CHANNEL_H
#define OFP_DEFAULT_CHANNEL_H

#include "ofp/channel.h"

namespace ofp { // <namespace ofp>
namespace impl { // <namespace impl>

class DefaultChannel : public Channel {
public:

	DefaultChannel(TCP_Connection *conn);

	DatapathID datapathID() const override {
		return dpid_;
	}

	UInt8 version() const override {
		return version_;
	}

	UInt32 nextXid() const override {
		return ++nextXid;
	}

	void write(const void *data, size_t length) override
	{
		conn_.write(data, length);
	}

	void messageReceived(Message *message);

private:
	TCP_Connection *conn_;
	DatapathID dpid_;
	UInt32 nextXid_;
	UInt8 version_;
	Channel *primary_;
};

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_DEFAULT_CHANNEL_H
