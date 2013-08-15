#ifndef OFP_CHANNELLISTENER_H
#define OFP_CHANNELLISTENER_H

#include "ofp/types.h"
#include <functional>

namespace ofp { // <namespace ofp>

class Channel;
class Message;
class Exception;

/**
 *  ChannelListener is an abstract class for listening to events and messages
 *  from a `Channel`.
 */
class ChannelListener {
public:

	using Factory = std::function<ChannelListener *()>;

	// Lifecycle:
	// 
	// Call onChannelUp when connection is up and channel is ready. Called one time. (See exception below.)
	// Call onChannelDown when connection goes down. Called one time. (See exception below.)
	// Call onMessage zero or more times.
	// Call onException one time. onChannelDown called immediately after. onException
	// is only called *after* onChannelUp been called.
	// 
	virtual void onChannelUp(Channel *channel) = 0;
	virtual void onChannelDown(Channel *channel) = 0;
	virtual void onMessage(const Message *message) = 0;
	virtual void onException(const Exception *exception) = 0;
	virtual void onTimer(UInt32 timerID) = 0;

	// When a controller identifies an incoming connection as an auxiliary 
	// connection, the driver will transparently tie the auxiliary connection
	// to the main connection. onChannelUp() and onChannelDown() will _NOT_ be
	// called for auxiliary connections that the controller did not initiate.
	// The listener may receive messages from the auxiliary connection; replies
	// can just be sent to message->source().
	// 
	// If an agent actively opens an auxiliary channel (via openAuxChannel),
	// onChannelUp() and onChannelDown() _WILL_ be called multiple times.
	//
	// A listener can determine the source channel of a message (main or 
	// auxiliary) by using Message::source(). A similar method exists for the 
	// Exception class. Exceptions on auxiliary connections will not shut down 
	// the main connection.
	
protected:
	// ChannelListeners must be allocated on the heap; never on the stack.
	// After detaching from a Channel, a ChannelListener may delete itself
	// using ChannelListener::dispose(this).

	virtual ~ChannelListener();

	static void dispose(ChannelListener *listener);

	friend class InternalChannel;
};


class AbstractChannelListener : public ChannelListener {
public:
	void onChannelUp(Channel *channel) override {}
	void onChannelDown(Channel *channel) override {}
	void onMessage(const Message *message) override {}
	void onException(const Exception *exception) override {}
	void onTimer(UInt32 timerID) override {}
};


} // </namespace ofp>


inline void ofp::ChannelListener::dispose(ChannelListener *listener)
{
	delete listener;
}


#endif // OFP_CHANNELLISTENER_H
