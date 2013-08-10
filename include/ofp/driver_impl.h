#ifndef OFP_DRIVER_IMPL_H
#define OFP_DRIVER_IMPL_H

#include "ofp/driver.h"
#include "ofp/boost_impl.h"

namespace ofp { // <namespace ofp>
namespace impl { // <namespace impl>

class TCP_Server;

class Driver_Impl {
public:
	Driver_Impl();

	void setProtocolVersions(const ProtocolVersions &versions);
	void setDriverOptions(const DriverOptions &options);

	void listen(Driver::Role role, UInt16 port, ChannelListener::Factory listenerFactory);
	void connect(Driver::Role role, const std::string &host, UInt16 port, ChannelListener::Factory listenerFactory);

	void run();
	void testRun();
	
	/////
	
	io_service &io() { return io_; }
	
private:
	io_service io_;
	std::vector<TCP_Server *> servers_;
};

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_DRIVER_IMPL_H
