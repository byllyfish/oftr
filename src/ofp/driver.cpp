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

namespace ofp { // <namespace ofp>

Driver::Driver(DriverOptions *options)
    : engine_{new sys::Engine{this, options}}
{
}

Driver::~Driver()
{
    delete engine_;
}

Deferred<Exception> Driver::listen(Role role, const Features *features, const IPv6Address &localAddress,
                         UInt16 localPort, ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory)
{
    return engine_->listen(role, features, localAddress, localPort, versions, listenerFactory);
}

Deferred<Exception> Driver::connect(Role role, const Features *features, const IPv6Address &remoteAddress,
                          UInt16 remotePort, ProtocolVersions versions,
                          ChannelListener::Factory listenerFactory)
{
    return engine_->connect(role, features, remoteAddress, remotePort, versions, listenerFactory);
}

void Driver::run()
{
    engine_->run();
}


void Driver::quit()
{
	engine_->quit();
}


} // </namespace ofp>
