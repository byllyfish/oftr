#ifndef OFP_CHANNEL_H
#define OFP_CHANNEL_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class Features;
class ChannelListener;

class Channel {
public:

	virtual ~Channel() {}

	virtual const Features *features() const = 0;
	virtual UInt8 protocolVersion() const = 0;
	virtual UInt32 nextXid() = 0;

	virtual void write(const void *data, size_t length) = 0;
	virtual void flush() = 0;

	virtual void close() = 0;

	// If you change the channel listener, you are responsible for disposing
	// of the old one.

	virtual ChannelListener *setChannelListener(ChannelListener *listener) = 0;

	// TODO: add a method to register a callback after a timeout.
};

} // </namespace ofp>

#endif // OFP_CHANNEL_H
