#ifndef OFP_SYS_CONNECTION_H
#define OFP_SYS_CONNECTION_H

#include "ofp/channel.h"
#include "ofp/channellistener.h"
#include "ofp/features.h"
#include "ofp/defaulthandshake.h"
#include "ofp/sys/connectiontimer.h"

namespace ofp { // <namespace ofp>

class Message;

namespace sys { // <namespace sys>
class Engine;

/**
 *  Connection is an interface for a channel that can receive messages
 *  posted from other Connections. This interface also supports binding
 *  auxillary connections to their main connection, and a main connection to a
 *  a linked list of auxiliary connections. It also supports a connection timer.
 */
OFP_BEGIN_IGNORE_PADDING
class Connection : public Channel {
public:
	Connection(Engine *engine, DefaultHandshake *handshake) :engine_{engine}, listener_{handshake}, handshake_{handshake} {}
	virtual ~Connection();

	Driver *driver() const override;

	UInt8 version() const override;
	void  setVersion(UInt8 version);

	const Features &features() const override;
	void setFeatures(const Features &features);

	DatapathID datapathID() const override { return features_.datapathID(); }
	UInt8 auxiliaryID() const override { return features_.auxiliaryID(); }
	
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

	DefaultHandshake *handshake() const { return handshake_; }
	void setHandshake(DefaultHandshake *handshake) { handshake_ = handshake; }

	void postMessage(Connection *source, Message *message);
	void postTimerExpired(ConnectionTimer *timer);
	void postIdle();
	void postDatapathID();

	sys::Engine *engine() const { return engine_; }
	
	void scheduleTimer(UInt32 timerID, milliseconds when, bool repeat) override;
	void cancelTimer(UInt32 timerID) override;

	void setStartingXid(UInt32 xid) override {
		nextXid_ = xid;
	}

private:
	sys::Engine *engine_;
	ChannelListener *listener_ = nullptr;
	DefaultHandshake *handshake_ = nullptr;
	Connection *mainConn_ = nullptr;
	Connection *nextAuxConn_ = nullptr;
	ConnectionTimerMap timers_;
	Features features_{};
	UInt32 nextXid_ = 0;
	UInt8 version_ = 0;
	bool dpidWasPosted_ = false;
};
OFP_END_IGNORE_PADDING

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_CONNECTION_H
