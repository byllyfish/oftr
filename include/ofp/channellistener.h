#ifndef OFP_CHANNELLISTENER_H
#define OFP_CHANNELLISTENER_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class Channel;
class Message;
class Error;

/**
 *  ChannelListener is an abstract class for listening to events and messages
 *  from a `Channel`.
 */
class ChannelListener {
public:

	using Factory = std::function<ChannelListener *()>;

	virtual ~ChannelListener() {}

	virtual void onChannelUp(Channel *channel) = 0;
	virtual void onChannelDown() = 0;
	virtual void onMessage(const Message *message) = 0;
	virtual void onError(const Error *error) = 0;

	// void onTimer(Channel *channel, Timer *timer);
	// Add a timer method to Channel which takes a function callback.
};

} // </namespace ofp>

#endif // OFP_CHANNELLISTENER_H
