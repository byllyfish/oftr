//  ===== ---- ofp/api/apiserver.h -------------------------*- C++ -*- =====  //
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
/// \brief Defines the api::ApiServer class for remote control of OpenFlow 
/// driver.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_API_APISERVER_H
#define OFP_API_APISERVER_H

#include "ofp/driver.h"
#include "ofp/datapathid.h"
#include <map>

namespace ofp {  // <namespace ofp>
namespace api {  // <namespace api>

class ApiConnection;
class ApiSession;

struct ApiLoopback;
struct ApiListenRequest;
struct ApiListenReply;
struct ApiConnectRequest;
struct ApiConnectReply;
struct ApiSetTimer;

OFP_BEGIN_IGNORE_PADDING

/// \brief Implements a server that lets a client control and monitor an 
/// OpenFlow driver over stdin/stdout.
/// The driver is controlled using YAML messages.
class ApiServer {
public:

	ApiServer(Driver *driver, int inputFD, int outputFD, Channel *defaultChannel = nullptr);
    ApiServer(Driver *driver, ApiSession *session, Channel *defaultChannel = nullptr);

	// Called by ApiConnection to update oneConn_.
	void onConnect(ApiConnection *conn);
	void onDisconnect(ApiConnection *conn);
	void onLoopback(ApiConnection *conn, ApiLoopback *loopback);

	// These methods are used to bridge ApiConnections to ApiChannelListeners.
    void onListenRequest(ApiConnection *conn, ApiListenRequest *listenReq);
    void onListenReply(ApiListenReply *listenReply);
    void onConnectRequest(ApiConnection *conn, ApiConnectRequest *connectReq);
    void onConnectReply(ApiConnectReply *connectReply);
	void onSetTimer(ApiConnection *conn, ApiSetTimer *setTimer);

	// These methods are used to bridge ApiChannelListeners to ApiConnections.
	void onChannelUp(Channel *channel);
	void onChannelDown(Channel *channel);
	void onMessage(Channel *channel, const Message *message);
	void onTimer(Channel *channel, UInt32 timerID);

	Channel *findChannel(const DatapathID &datapathId);
	
    sys::Engine *engine() { return engine_; }
    
private:
	using DatapathMap = std::map<DatapathID,Channel*>;

    sys::Engine *engine_;
    ApiConnection *oneConn_ = nullptr;
    DatapathMap datapathMap_;
    Channel *defaultChannel_ = nullptr;

    void registerChannel(Channel *channel);
    void unregisterChannel(Channel *channel);
};

OFP_END_IGNORE_PADDING

} // </namespace api>
} // </namespace ofp>

#endif // OFP_API_APISERVER_H
