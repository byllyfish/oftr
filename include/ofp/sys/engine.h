//  ===== ---- ofp/sys/engine.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the sys::Engine class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_ENGINE_H
#define OFP_SYS_ENGINE_H

#include "ofp/driver.h"
#include "ofp/sys/boost_asio.h"
#include "ofp/defaulthandshake.h"
#include "ofp/channel.h"
#include "ofp/datapathid.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

class Server;
class TCP_Server;
class TCP_Connection; // FIXME can I use Connection here?

OFP_BEGIN_IGNORE_PADDING

class Engine {
public:
     Engine(Driver *driver, DriverOptions *options);
    ~Engine();

    Deferred<Exception> listen(Driver::Role role, const Features *features,
                               const IPv6Address &localAddress,
                               UInt16 localPort, ProtocolVersions versions,
                               ChannelListener::Factory listenerFactory);

    Deferred<Exception> connect(Driver::Role role, const Features *features,
                                const IPv6Address &remoteAddress,
                                UInt16 remotePort, ProtocolVersions versions,
                                ChannelListener::Factory listenerFactory);

    void reconnect(DefaultHandshake *handshake, const Features *features, const IPv6Address &remoteAddress, UInt16 remotePort, milliseconds delay);

    void run();
    void quit();
    bool isRunning() const { return isRunning_; }
    
    void openAuxChannel(UInt8 auxID, Channel::Transport transport, TCP_Connection *mainConnection);

    io_service &io()
    {
        return io_;
    }

    Driver *driver() const
    {
        return driver_;
    }

    void postDatapathID(Connection *channel);
    void releaseDatapathID(Connection *channel);

    void registerServer(Server *server);
    void releaseServer(Server *server);

private:
    Driver *driver_;

    using DatapathMap = std::map<DatapathID,Connection *>;
    using ServerList = std::vector<Server *>;

    DatapathMap dpidMap_;
    ServerList serverList_;
    
    // The io_service must be the first object to be destroyed when engine
    // destructor runs.
    io_service io_;
    bool isRunning_ = false;
};

OFP_END_IGNORE_PADDING

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_ENGINE_H
