#ifndef OFP_DRIVER_H
#define OFP_DRIVER_H

#include "ofp/channellistener.h"
#include "ofp/ipv6address.h"
#include "ofp/protocolversions.h"

namespace ofp { // <namespace ofp>

namespace impl {
	class Driver_Impl;
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
	void connect(Role role, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);

	void run();

private:
	impl::Driver_Impl *impl_;
};

} // </namespace ofp>


#endif // OFP_DRIVER_H
