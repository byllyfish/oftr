#ifndef OFP_CHANNELLISTENER_H
#define OFP_CHANNELLISTENER_H

#include "ofp/types.h"

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
	// Call onChannelUp when connection is up and channel is ready. Called one time.
	// Call onChannelDown when connection goes down. Called one time.
	// Call onMessage zero or more times.
	// Call onException one time. onChannelDown called immediately after. onException
	// is only called *after* onChannelUp been called.
	// 
	virtual void onChannelUp(Channel *channel) = 0;
	virtual void onChannelDown() = 0;
	virtual void onMessage(const Message *message) = 0;

	// Default action? log error? close channel?
	// No exceptions will be reported until after onChannelUp has been called.
	virtual void onException(const Exception *exception) = 0;

	// void onTimer(Channel *channel, Timer *timer);
	// Add a timer method to Channel which takes a function callback.
	
	static void dispose(ChannelListener *listener);
	
protected:
	// ChannelListeners must be allocated on the heap; never on the stack.
	// After detaching from a Channel, a ChannelListener may delete itself
	// using ChannelListener::dispose(this).

	virtual ~ChannelListener() {}
};


class AbstractChannelListener : public ChannelListener {
public:
	void onChannelUp(Channel *channel) override {}
	void onChannelDown() override {}
	void onMessage(const Message *message) override {}
	void onException(const Exception *exception) override {}
};


} // </namespace ofp>


inline void ofp::ChannelListener::dispose(ChannelListener *listener)
{
	delete listener;
}


#endif // OFP_CHANNELLISTENER_H
