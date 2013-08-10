#include "ofp/driver_impl.h"
#include "ofp/log.h"
#include "ofp/tcp_server_impl.h"


ofp::impl::Driver_Impl::Driver_Impl()
{
	log::debug(__PRETTY_FUNCTION__);
}

void ofp::impl::Driver_Impl::setProtocolVersions(const ProtocolVersions &versions)
{
	log::debug(__PRETTY_FUNCTION__);


}

void ofp::impl::Driver_Impl::setDriverOptions(const DriverOptions &options)
{
	log::debug(__PRETTY_FUNCTION__);

}

void ofp::impl::Driver_Impl::listen(Driver::Role role, UInt16 port, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);

	tcp::endpoint endpt{tcp::v6(), port};

	TCP_Server *server = new TCP_Server{this, endpt};
	servers_.push_back(server);
}


void ofp::impl::Driver_Impl::connect(Driver::Role role, const std::string &host, UInt16 port, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);
}

void ofp::impl::Driver_Impl::run()
{
	log::debug(__PRETTY_FUNCTION__);

	io_.run();
}


void ofp::impl::Driver_Impl::testRun() 
{
}