#include "ofp/driver.h"
#include "ofp/impl/engine.h"

ofp::Driver::Driver(DriverOptions *options)
    : engine_{new impl::Engine{options}}
{
}

ofp::Driver::~Driver()
{
    delete engine_;
}

void ofp::Driver::listen(Role role, const IPv6Address &localAddress,
                         UInt16 localPort, ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory)
{
    engine_->listen(role, localAddress, localPort, versions, listenerFactory);
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

