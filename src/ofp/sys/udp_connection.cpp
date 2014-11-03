//  ===== ---- ofp/sys/udp_connection.cpp ------------------*- C++ -*- =====  //
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
/// \brief Implements sys::UDP_Connection class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/sys/udp_connection.h"
#include "ofp/defaulthandshake.h"
#include "ofp/sys/udp_server.h"
#include "ofp/log.h"

using ofp::sys::UDP_Connection;
using ofp::IPv6Endpoint;

UDP_Connection::UDP_Connection(UDP_Server *server, ChannelMode mode,
                               ProtocolVersions versions,
                               ChannelListener::Factory factory)
    : Connection{server->engine(),
                 new DefaultHandshake{this, mode, versions, factory}},
      server_{server} {}

UDP_Connection::~UDP_Connection() {
  if (connectionId()) {
    log::info("Close UDP connection", std::make_pair("connid", connectionId()));
    server_->remove(this);
  }
}

void UDP_Connection::connect(const udp::endpoint &remoteEndpt) {
  remoteEndpt_ = remoteEndpt;
  server_->add(this);
  log::info("Establish UDP connection", localEndpoint(), "-->",
            remoteEndpoint(), std::make_pair("connid", connectionId()));
  channelUp();
}

void UDP_Connection::accept(const udp::endpoint &remoteEndpt) {
  remoteEndpt_ = remoteEndpt;
  server_->add(this);
  log::info("Accept UDP connection", localEndpoint(), "<--", remoteEndpoint(),
            std::make_pair("connid", connectionId()));
  channelUp();
}

IPv6Endpoint UDP_Connection::remoteEndpoint() const {
  return convertEndpoint<udp>(remoteEndpt_);
}

IPv6Endpoint UDP_Connection::localEndpoint() const {
  return server_->localEndpoint();
}

void UDP_Connection::write(const void *data, size_t length) {
  server_->write(data, length);
}

void UDP_Connection::flush() { server_->flush(remoteEndpt_, connectionId()); }

void UDP_Connection::shutdown() { delete this; }

void UDP_Connection::channelUp() {
  assert(channelListener());
  channelListener()->onChannelUp(this);
}
