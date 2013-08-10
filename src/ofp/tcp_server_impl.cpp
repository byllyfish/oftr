#include "ofp/tcp_server_impl.h"
#include "ofp/driver_impl.h"
#include "ofp/log.h"

ofp::impl::TCP_Server::TCP_Server(Driver_Impl *driver, const tcp::endpoint &endpt)
    : driver_{driver},
      acceptor_{driver->io(), endpt},
      socket_{driver->io()}
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
            std::make_shared<TCP_Connection>(this, std::move(socket_))
                ->start();
        }

        asyncAccept();
    });
}
