#include "ofp/sys/tcp_server.h"
#include "ofp/sys/engine.h"
#include "ofp/log.h"

ofp::sys::TCP_Server::TCP_Server(Engine *engine, Driver::Role role,
                                  const tcp::endpoint &endpt,
                                  ProtocolVersions versions,
                                  ChannelListener::Factory listenerFactory)
    : engine_{engine}, acceptor_{engine->io(), endpt}, socket_{engine->io()},
      role_{role}, versions_{versions}, factory_{listenerFactory}
{
    log::debug(__PRETTY_FUNCTION__);

    asyncAccept();
}

ofp::sys::TCP_Server::~TCP_Server()
{
    log::debug(__PRETTY_FUNCTION__);
}

void ofp::sys::TCP_Server::asyncAccept()
{
    log::debug(__PRETTY_FUNCTION__);

    acceptor_.async_accept(socket_, [this](error_code err) {
        if (!err) {
            auto conn = std::make_shared<TCP_Connection>(engine_, std::move(socket_), role_,
                                             versions_, factory_);
            conn->asyncAccept();
            
        } else {
            Exception exc = makeException(err);
            log::info("exception ", exc.toString());
        }

        asyncAccept();
    });
}
