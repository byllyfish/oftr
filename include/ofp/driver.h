//  ===== ---- ofp/driver.h --------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Driver class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_DRIVER_H
#define OFP_DRIVER_H

#include "ofp/channellistener.h"
#include "ofp/ipv6address.h"
#include "ofp/protocolversions.h"
#include "ofp/deferred.h"

namespace ofp { // <namespace ofp>

namespace sys {
	class Engine;
}

struct DriverOptions {
	// Settings for reliable connections
	milliseconds agentReconnectDelay = 500_ms;  // FIXME - support
	milliseconds agentIdleInterval = 5000_ms;
	milliseconds agentEchoTimeout = 1000_ms;
	milliseconds controllerIdleInterval = 5500_ms;
	milliseconds controllerEchoTimeout = 1000_ms;

	/// Platform-specific context for TLS implementation (certificates, etc).
	/// Pass a pointer to a `boost::asio::ssl::context`.
	void *tlsContext = nullptr;
};

class Features;
class ThreadManager;


class Driver {
public:

	enum Role {
		Agent = 0,
		Controller,
		Auxiliary  // for internal use only FIXME (assert in listen and connect)
	};

	enum {
		DefaultPort = 6633
	};

	 Driver(DriverOptions *options = nullptr);
	~Driver();

	Deferred<Exception> listen(Role role, const Features *features, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);

	Deferred<Exception> connect(Role role, const Features *features, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory);

	void run();

	/// \brief Tells the driver to stop running.
	void quit();

private:
	sys::Engine *engine_;
	log::Lifetime lifetime{"Driver"};
};

} // </namespace ofp>


#endif // OFP_DRIVER_H
