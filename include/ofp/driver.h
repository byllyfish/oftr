#ifndef OFP_DRIVER_H
#define OFP_DRIVER_H

#include "ofp/channellistener.h"
#include "ofp/ipv6address.h"
#include "ofp/protocolversions.h"
#include "ofp/deferred.h"

namespace ofp { // <namespace ofp>

namespace impl {
	class Engine;
}

class DriverOptions;

class Driver {
public:

	enum Role {
		Agent = 0,
		Controller
	};

	enum {
		DefaultPort = 6663
	};

	 Driver(DriverOptions *options = nullptr);
	~Driver();

	// FIXME Should return Exception?
	// // Use Result<bool> or Exception
	void listen(Role role, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);

	// Connect to specified address.
	// If there is an error establishing the connection, or the connection is established 

	Deferred<Exception> connect(Role role, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);

	void run();

private:
	impl::Engine *engine_;
	log::Lifetime lifetime{"Driver"};
};

} // </namespace ofp>


#endif // OFP_DRIVER_H
