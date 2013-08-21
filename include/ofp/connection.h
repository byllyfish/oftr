#ifndef OFP_CONNECTION_H
#define OFP_CONNECTION_H

#include "ofp/channel.h"
#include "ofp/channellistener.h"
#include "ofp/features.h"

namespace ofp { // <namespace ofp>

namespace impl { // <namespace impl>
class Engine;
} // </namespace impl>
class Message;

/**
 *  Connection is an interface for a channel that can receive messages
 *  posted from other Connections. This interface also supports binding
 *  auxillary connections to their main connection, and a main connection to a
 *  a linked list of auxiliary connections.
 */
OFP_BEGIN_IGNORE_PADDING
class Connection : public Channel {
public:
	Connection(impl::Engine *engine, ChannelListener *listener) :engine_{engine}, listener_{listener} {}
	virtual ~Connection();

	Driver *driver() const override;

	UInt8 version() const override;
	void  setVersion(UInt8 version);

	const Features &features() const override;
	void setFeatures(const Features &features);

	Connection *mainConnection() const 
	{ return mainConn_; }

	void setMainConnection(Connection *channel) {
		mainConn_ = channel;
	}

	Connection *nextAuxiliaryConnection() const {
		return nextAuxConn_;
	}

	void setNextAuxiliaryConnection(Connection *channel) {
		nextAuxConn_ = channel;
	}

	UInt32 nextXid() override {
		return ++nextXid_;
	}

	ChannelListener *channelListener() const override {
		return listener_;
	}
	void setChannelListener(ChannelListener *listener) override {
		listener_ = listener;
	}

	void postMessage(Connection *source, Message *message);

	impl::Engine *engine() { return engine_; }

	void scheduleTimer(UInt32 timerID, std::chrono::milliseconds when) {
		// TODO
	}

	void cancelTimer(UInt32 timerID) {
		// TODO
	}

private:
	impl::Engine *engine_;
	ChannelListener *listener_ = nullptr;
	Connection *mainConn_ = nullptr;
	Connection *nextAuxConn_ = nullptr;
	Features features_{};
	UInt32 nextXid_ = 0;
	UInt8 version_ = 0;
};
OFP_END_IGNORE_PADDING

} // </namespace ofp>

#endif // OFP_CONNECTION_H
