#ifndef OFP_CHANNEL_H
#define OFP_CHANNEL_H

#include "ofp/writable.h"
#include <chrono>

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

	virtual void scheduleTimer(UInt32 timerID, std::chrono::milliseconds when) = 0;
	virtual void cancelTimer(UInt32 timerID) = 0;

	// The failure to open an auxiliary channel will be reported to the main listener via onException.
	// Returns the auxiliary ID of the connection? This method is intended to be used by agents who
	// want to open an auxiliary channel to a controller. NOT IMPLEMENTED IN THIS RELEASE.
	virtual void openAuxChannel() = 0;
};

} // </namespace ofp>

#endif // OFP_CHANNEL_H
