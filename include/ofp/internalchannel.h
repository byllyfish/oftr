#ifndef OFP_INTERNALCHANNEL_H
#define OFP_INTERNALCHANNEL_H

#include "ofp/channel.h"
#include "ofp/features.h"
#include "ofp/impl/engine.h"

namespace ofp { // <namespace ofp>

class Message;

/**
 *  InternalChannel is an interface for a channel that can receive messages
 *  posted from other InternalChannels. This interface also supports binding
 *  auxillary connections to their main connection, and a main connection to a
 *  a linked list of auxiliary connections.
 */
OFP_BEGIN_IGNORE_PADDING
class InternalChannel : public Channel {
public:
	InternalChannel(impl::Engine *engine, ChannelListener *listener) :engine_{engine}, listener_{listener} {}
	virtual ~InternalChannel();

	Driver *driver() const override {
    	return engine_->driver();
    }

	UInt8 version() const override;
	void  setVersion(UInt8 version);

	const Features &features() const override;
	void setFeatures(const Features &features);

	InternalChannel *mainConnection() const 
	{ return mainConn_; }

	void setMainConnection(InternalChannel *channel) {
		mainConn_ = channel;
	}

	InternalChannel *nextAuxiliaryConnection() const {
		return nextAuxConn_;
	}

	void setNextAuxiliaryConnection(InternalChannel *channel) {
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

	void postMessage(InternalChannel *source, Message *message) {
		if (listener_) {
			listener_->onMessage(message);
		} else {
			log::info("No listener. Message dropped.");
		}
	}

	impl::Engine *engine() { return engine_; }

private:
	impl::Engine *engine_;
	ChannelListener *listener_ = nullptr;
	InternalChannel *mainConn_ = nullptr;
	InternalChannel *nextAuxConn_ = nullptr;
	Features features_{};
	UInt32 nextXid_ = 0;
	UInt8 version_ = 0;
};
OFP_END_IGNORE_PADDING

} // </namespace ofp>

#endif // OFP_INTERNALCHANNEL_H
