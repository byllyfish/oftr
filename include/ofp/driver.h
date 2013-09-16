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

namespace sys { // <namespace sys>
class Engine;
} // </namespace ofp>

class Features;

struct DriverOptions {
    /// Platform-specific context for TLS implementation (certificates, etc).
    /// Pass a pointer to a `boost::asio::ssl::context`.
    void *tlsContext = nullptr;
};

class Driver {
public:

    enum Role {
        Agent = 0,
        Controller,
        Auxiliary // for internal use only
    };

    enum {
        DefaultPort = 6633
    };

    Driver(DriverOptions *options = nullptr);
    ~Driver();

    Deferred<Exception> listen(Role role, const Features *features,
                               const IPv6Address &localAddress,
                               UInt16 localPort, ProtocolVersions versions,
                               ChannelListener::Factory listenerFactory);

    Deferred<Exception> connect(Role role, const Features *features,
                                const IPv6Address &remoteAddress,
                                UInt16 remotePort, ProtocolVersions versions,
                                ChannelListener::Factory listenerFactory);

    void run();

    /// \brief Tells the driver to stop running.
    void quit();

    sys::Engine *engine()
    {
        return engine_;
    }

private:
    sys::Engine *engine_;
    log::Lifetime lifetime{"Driver"};
};

} // </namespace ofp>

#endif // OFP_DRIVER_H
