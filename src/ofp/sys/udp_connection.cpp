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


ofp::sys::UDP_Connection::UDP_Connection(UDP_Server *server, Driver::Role role, ProtocolVersions versions, udp::endpoint remoteEndpt) 
: Connection{server->engine(), new DefaultHandshake{this, role, versions, nullptr}}, server_{server}, remoteEndpt_{remoteEndpt}
{
	server_->add(this);
}

ofp::sys::UDP_Connection::~UDP_Connection() {
	server_->remove(this);
}

void ofp::sys::UDP_Connection::write(const void *data, size_t length) {
	server_->write(data, length);
}

void ofp::sys::UDP_Connection::flush() {
	server_->flush(remoteEndpt_);
}

void ofp::sys::UDP_Connection::shutdown() 
{
	log::info("close the 'connection' to this udp endpoint");

	// call delete this?
}
