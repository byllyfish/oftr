#include "ofp/driver.h"
#include "ofp/sys/engine.h"

namespace ofp { // <namespace ofp>

Driver::Driver(DriverOptions *options)
    : engine_{new sys::Engine{this, options}}
{
}

Driver::~Driver()
{
    delete engine_;
}

Deferred<Exception> Driver::listen(Role role, const Features *features, const IPv6Address &localAddress,
                         UInt16 localPort, ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory)
{
    return engine_->listen(role, features, localAddress, localPort, versions, listenerFactory);
}

Deferred<Exception> Driver::connect(Role role, const Features *features, const IPv6Address &remoteAddress,
                          UInt16 remotePort, ProtocolVersions versions,
                          ChannelListener::Factory listenerFactory)
{
    return engine_->connect(role, features, remoteAddress, remotePort, versions, listenerFactory);
}

void Driver::run()
{
    engine_->run();
}


void Driver::quit()
{
	engine_->quit();
}


} // </namespace ofp>
