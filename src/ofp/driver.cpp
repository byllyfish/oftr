#include "ofp/driver.h"
#include "ofp/driver_impl.h"

ofp::Driver::Driver() : impl_{new impl::Driver_Impl}
{
}

ofp::Driver::~Driver()
{
    delete impl_;
}

void ofp::Driver::setProtocolVersions(const ProtocolVersions &versions)
{
    impl_->setProtocolVersions(versions);
}

void ofp::Driver::setDriverOptions(const DriverOptions &options)
{
    impl_->setDriverOptions(options);
}

void ofp::Driver::listen(Role role, UInt16 port,
                         ChannelListener::Factory listenerFactory)
{
    impl_->listen(role, port, listenerFactory);
}

void ofp::Driver::connect(Role role, const std::string &host, UInt16 port,
                          ChannelListener::Factory listenerFactory)
{
    impl_->connect(role, host, port, listenerFactory);
}

void ofp::Driver::run()
{
    impl_->run();
}

void ofp::Driver::testRun()
{
	impl_->testRun();
}
