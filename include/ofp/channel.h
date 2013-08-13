#ifndef OFP_CHANNEL_H
#define OFP_CHANNEL_H

#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

class Driver;
class Features;
class ChannelListener;

class Channel : public Writable {
public:
	virtual ~Channel();
	
	virtual Driver *driver() const = 0;
	virtual const Features &features() const = 0;

	virtual ChannelListener *channelListener() const = 0;
	virtual void setChannelListener(ChannelListener *listener) = 0;

	// The failure to open an auxiliary channel will be reported to the main listener via onException.
	// Returns the auxiliary ID of the connection.
	virtual void openAuxChannel() = 0;
};

} // </namespace ofp>

#endif // OFP_CHANNEL_H
