#include "ofp/driver.h"
#include "ofp/sys/engine.h"

ofp::Driver::Driver(DriverOptions *options)
    : engine_{new sys::Engine{this, options}}
{
}

ofp::Driver::~Driver()
{
    delete engine_;
}

ofp::Deferred<ofp::Exception> ofp::Driver::listen(Role role, const IPv6Address &localAddress,
                         UInt16 localPort, ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory)
{
    return engine_->listen(role, localAddress, localPort, versions, listenerFactory);
}

ofp::Deferred<ofp::Exception> ofp::Driver::connect(Role role, const IPv6Address &remoteAddress,
                          UInt16 remotePort, ProtocolVersions versions,
                          ChannelListener::Factory listenerFactory)
{
    return engine_->connect(role, remoteAddress, remotePort, versions, listenerFactory);
}

void ofp::Driver::run()
{
    engine_->run();
}

