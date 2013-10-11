//  ===== ---- ofp/driver.cpp ------------------------------*- C++ -*- =====  //
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
/// \brief Implements Driver class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/driver.h"
#include "ofp/sys/engine.h"

using namespace ofp;

Driver::Driver(DriverOptions *options) : engine_{new sys::Engine{this, options}}
{
}

Driver::~Driver()
{
    delete engine_;
}

Deferred<Exception> Driver::listen(Role role, const Features *features,
                                   const IPv6Endpoint &localEndpoint,
                                   ProtocolVersions versions,
                                   ChannelListener::Factory listenerFactory)
{
    return engine_->listen(role, features, localEndpoint, versions,
                           listenerFactory);
}

Deferred<Exception> Driver::connect(Role role, const Features *features,
                                    const IPv6Endpoint &remoteEndpoint,
                                    ProtocolVersions versions,
                                    ChannelListener::Factory listenerFactory)
{
    return engine_->connect(role, features, remoteEndpoint, versions,
                            listenerFactory);
}

void Driver::run()
{
    engine_->run();
}

void Driver::stop()
{
    engine_->stop();
}

void Driver::installSignalHandlers()
{
    engine_->installSignalHandlers();
}
