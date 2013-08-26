#ifndef OFP_DRIVER_H
#define OFP_DRIVER_H

#include "ofp/channellistener.h"
#include "ofp/ipv6address.h"
#include "ofp/protocolversions.h"
#include "ofp/deferred.h"
#include <chrono>

namespace ofp { // <namespace ofp>

namespace sys {
	class Engine;
}

class DriverOptions;
class ThreadManager;


class Driver {
public:

	enum Role {
		Agent = 0,
		Controller
	};

	enum {
		DefaultPort = 6633
	};

	 Driver(DriverOptions *options = nullptr);
	~Driver();

	Deferred<Exception> listen(Role role, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);

	Deferred<Exception> connect(Role role, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);

	void run();

private:
	sys::Engine *engine_;
	log::Lifetime lifetime{"Driver"};
};

} // </namespace ofp>


#endif // OFP_DRIVER_H
