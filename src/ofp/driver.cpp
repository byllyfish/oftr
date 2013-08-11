#include "ofp/driver.h"
#include "ofp/impl/driver_impl.h"

ofp::Driver::Driver(DriverOptions *options)
    : impl_{new impl::Driver_Impl{options}}
{
}

ofp::Driver::~Driver()
{
    delete impl_;
}

void ofp::Driver::listen(Role role, const IPv6Address &localAddress,
                         UInt16 localPort, ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory)
{
    impl_->listen(role, localAddress, localPort, versions, listenerFactory);
}

void ofp::Driver::connect(Role role, const IPv6Address &remoteAddress,
                          UInt16 remotePort, ProtocolVersions versions,
                          ChannelListener::Factory listenerFactory)
{
    impl_->connect(role, remoteAddress, remotePort, versions, listenerFactory);
}

void ofp::Driver::run()
{
    impl_->run();
}

