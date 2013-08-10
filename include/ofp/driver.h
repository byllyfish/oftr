#ifndef OFP_DRIVER_H
#define OFP_DRIVER_H

#include "ofp/channellistener.h"

namespace ofp { // <namespace ofp>

namespace impl {
	class Driver_Impl;
}
class Channel;
class ProtocolVersions;
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

	 Driver();
	~Driver();

	void setProtocolVersions(const ProtocolVersions &versions);
	void setDriverOptions(const DriverOptions &options);

	void listen(Role role, UInt16 port, ChannelListener::Factory listenerFactory);
	void connect(Role role, const std::string &host, UInt16 port, ChannelListener::Factory listenerFactory);

	void run();
	void testRun();

private:
	impl::Driver_Impl *impl_;
};

} // </namespace ofp>


#endif // OFP_DRIVER_H
