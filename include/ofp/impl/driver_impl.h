#ifndef OFP_DRIVER_IMPL_H
#define OFP_DRIVER_IMPL_H

#include "ofp/driver.h"
#include "ofp/impl/boost_impl.h"

namespace ofp { // <namespace ofp>
namespace impl { // <namespace impl>

class TCP_Server;

class Driver_Impl {
public:
	Driver_Impl(DriverOptions *options);

	void listen(Driver::Role role, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);
	void connect(Driver::Role role, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);

	void run();
	
	/////
	
	io_service &io() { return io_; }
	
private:
	io_service io_;
	std::vector<TCP_Server *> servers_;
};

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_DRIVER_IMPL_H
