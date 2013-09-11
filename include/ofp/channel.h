//  ===== ---- ofp/channel.h -------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the abstract Channel class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_CHANNEL_H
#define OFP_CHANNEL_H

#include "ofp/writable.h"
#include "ofp/ipv6address.h"
#include "ofp/datapathid.h"

namespace ofp { // <namespace ofp>

class Driver;
class Features;
class ChannelListener;
class DatapathID;

class Channel : public Writable {
public:
	virtual ~Channel();
	
	virtual Driver *driver() const = 0;
	virtual const Features &features() const = 0;
	virtual DatapathID datapathId() const = 0;
	virtual UInt8 auxiliaryId() const = 0;
	virtual IPv6Address remoteAddress() const = 0;
	virtual UInt16 remotePort() const = 0;
	virtual void shutdown() = 0;
	
	virtual ChannelListener *channelListener() const = 0;
	virtual void setChannelListener(ChannelListener *listener) = 0;

	virtual void scheduleTimer(UInt32 timerID, milliseconds when, bool repeat = false) = 0;
	virtual void cancelTimer(UInt32 timerID) = 0;

	// The following methods are provided for OpenFlow agents.
	// The failure to open an auxiliary channel will be reported to the main listener via onException.
	
	enum class Transport {
		TCP,
		UDP,
		TLS,
		DTLS
	};

	virtual Transport transport() const = 0;
	virtual void openAuxChannel(UInt8 auxID, Transport transport) = 0;
	virtual Channel *findAuxChannel(UInt8 auxID) const = 0;

	virtual void setStartingXid(UInt32 xid) = 0;

};


std::ostream &operator<<(std::ostream &os, Channel *channel);

inline std::ostream &operator<<(std::ostream &os, Channel *channel)
{
	return os << "[Channel to=" << channel->remoteAddress() << ':' << channel->remotePort() << ']';
}

} // </namespace ofp>

#endif // OFP_CHANNEL_H
