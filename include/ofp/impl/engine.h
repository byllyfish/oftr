#ifndef OFP_IMPL_ENGINE_H
#define OFP_IMPL_ENGINE_H

#include "ofp/driver.h"
#include "ofp/impl/boost_asio.h"

namespace ofp { // <namespace ofp>
namespace impl { // <namespace impl>

class TCP_Server;

class Engine {
public:
	Engine(Driver *driver, DriverOptions *options);

	Deferred<Exception> listen(Driver::Role role, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);
	Deferred<Exception> connect(Driver::Role role, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);

	void run();
	
	// TODO
	void openAuxChannel(Channel *main, tcp::endpoint endpt) {}
	void openAuxChannel(Channel *main, udp::endpoint endpt) {}

	io_service &io() { return io_; }
	Driver *driver() const { return driver_; }
	
private:
	io_service io_;
	Driver *driver_;
};

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_IMPL_ENGINE_H
