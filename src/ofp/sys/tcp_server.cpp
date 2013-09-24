//  ===== ---- ofp/sys/tcp_server.cpp ----------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements TCP_Server class.
//  ===== ------------------------------------------------------------ =====  //

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
