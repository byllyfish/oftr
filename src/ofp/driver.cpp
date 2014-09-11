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

Driver::Driver() : engine_{new sys::Engine{this}} {}

Driver::~Driver() { delete engine_; }

std::error_code
Driver::configureTLS(const std::string &privateKeyFile,
                     const std::string &certificateFile,
                     const std::string &certificateAuthorityFile,
                     const char *privateKeyPassword) {
  return engine_->configureTLS(privateKeyFile, certificateFile,
                               certificateAuthorityFile, privateKeyPassword);
}

UInt64 Driver::listen(ChannelMode mode, const IPv6Endpoint &localEndpoint,
                                   ProtocolVersions versions,
                                   ChannelListener::Factory listenerFactory, std::error_code &error) {
  return engine_->listen(mode, localEndpoint, versions, listenerFactory, error);
}

Deferred<std::error_code> Driver::connect(ChannelMode mode,
                                    const IPv6Endpoint &remoteEndpoint,
                                    ProtocolVersions versions,
                                    ChannelListener::Factory listenerFactory) {
  return engine_->connect(mode, remoteEndpoint, versions, listenerFactory);
}

void Driver::run() { engine_->run(); }

void Driver::stop(Milliseconds timeout) { engine_->stop(timeout); }
