#include "ofp/sys/tcp_server.h"
#include "ofp/sys/engine.h"
#include "ofp/log.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

TCP_Server::TCP_Server(Engine *engine, Driver::Role role,
                       const Features *features, const tcp::endpoint &endpt,
                       ProtocolVersions versions,
                       ChannelListener::Factory listenerFactory)
    : engine_{engine}, acceptor_{engine->io(), endpt}, socket_{engine->io()},
      role_{role}, versions_{versions}, factory_{listenerFactory}
{
    if (features) {
        features_ = *features;
    }

    asyncAccept();

    engine_->registerServer(this);

    log::info("Start TCP listening on", endpt);
}

TCP_Server::~TCP_Server()
{
    error_code err;
    tcp::endpoint endpt = acceptor_.local_endpoint(err);

    log::info("Stop TCP listening on", endpt);
    engine_->releaseServer(this);
}

void TCP_Server::asyncAccept()
{
    acceptor_.async_accept(socket_, [this](error_code err) {
        // N.B. ASIO still sends a cancellation error even after
        // async_accept() throws an exception. Check for cancelled operation
        // first; our TCP_Server instance will have been destroyed.
        if (isAsioCanceled(err))
            return;

        log::Lifetime lifetime("async_accept callback");
        if (!err) {
            auto conn = std::make_shared<TCP_Connection>(
                engine_, std::move(socket_), role_, versions_, factory_);
            conn->setFeatures(features_);
            conn->asyncAccept();

        } else {
            Exception exc = makeException(err);
            log::error("Error in TCP_Server.asyncAcept:", exc.toString());
        }

        asyncAccept();
    });
}

} // </namespace sys>
} // </namespace ofp>
