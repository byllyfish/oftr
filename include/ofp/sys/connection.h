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
	Connection(Engine *engine, DefaultHandshake *handshake) :engine_{engine}, listener_{handshake}, handshake_{handshake}, mainConn_{this} {}
	virtual ~Connection();

	Driver *driver() const override;

	UInt8 version() const override { return version_; }
	void  setVersion(UInt8 version) { version_ = version; }

	const Features &features() const override { return features_; }
	void setFeatures(const Features &features) { features_ = features; }

	DatapathID datapathId() const override { return features_.datapathId(); }
	UInt8 auxiliaryId() const override { return features_.auxiliaryId(); }
	
	Connection *mainConnection() const 
	{ return mainConn_; }

	void setMainConnection(Connection *channel);

	//Connection *nextAuxiliaryConnection() const {
	//	return nextAuxConn_;
	//}

	//void setNextAuxiliaryConnection(Connection *channel) {
	//	nextAuxConn_ = channel;
	//}

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
	using AuxiliaryList = std::vector<Connection*>;

	sys::Engine *engine_;
	ChannelListener *listener_ = nullptr;
	DefaultHandshake *handshake_ = nullptr;
	Connection *mainConn_;
	AuxiliaryList auxList_;
	ConnectionTimerMap timers_;
	Features features_;
	UInt32 nextXid_ = 0;
	UInt8 version_ = 0;
	bool dpidWasPosted_ = false;
};
OFP_END_IGNORE_PADDING

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_CONNECTION_H
