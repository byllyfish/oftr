#include "ofp/impl/tcp_server.h"
#include "ofp/impl/driver_impl.h"
#include "ofp/log.h"

ofp::impl::TCP_Server::TCP_Server(Driver_Impl *driver, Driver::Role role,
                                  const tcp::endpoint &endpt,
                                  ProtocolVersions versions,
                                  ChannelListener::Factory listenerFactory)
    : driver_{driver}, acceptor_{driver->io(), endpt}, socket_{driver->io()},
      role_{role}, versions_{versions}, factory_{listenerFactory}
{
    log::debug(__PRETTY_FUNCTION__);

    asyncAccept();
}

ofp::impl::TCP_Server::~TCP_Server()
{
    log::debug(__PRETTY_FUNCTION__);
}

void ofp::impl::TCP_Server::asyncAccept()
{
    log::debug(__PRETTY_FUNCTION__);

    acceptor_.async_accept(socket_, [this](error_code err) {
        if (!err) {
            std::make_shared<TCP_Connection>(this, std::move(socket_), role_,
                                             versions_, factory_)->start();
        }

        asyncAccept();
    });
}
