//  ===== ---- ofp/yaml/apiserver.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the yaml::ApiServer class for remote control of OpenFlow 
/// driver.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_APISERVER_H
#define OFP_YAML_APISERVER_H

#include "ofp/yaml/yllvm.h"
#include "ofp/sys/boost_asio.h"
#include "ofp/driver.h"
#include "ofp/yaml/apievents.h"

OFP_BEGIN_IGNORE_PADDING

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>

class ApiConnection;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   A p i S e r v e r
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //

/// \brief Implments a TCP server that lets a client control and monitor an 
/// OpenFlow driver.
/// The driver is controlled using YAML messages.
class ApiServer {
public:
	/// \brief Run server on specified local address and port. 
	///  Pass `IPv6Address{}` to specify `all` local addresses.
	static void run(const IPv6Address &localAddress, UInt16 localPort);

	// Called by ApiConnection to update oneConn_.
	void onConnect(ApiConnection *conn);
	void onDisconnect(ApiConnection *conn);

	// These methods are used to bridge ApiConnections to ApiChannelListeners.
    void onListenRequest(ApiConnection *conn, ApiListenRequest *listenReq);
    void onListenReply(ApiListenReply *listenReply);
	void onSetTimer(ApiConnection *conn, ApiSetTimer *setTimer);

	// These methods are used to bridge ApiChannelListeners to ApiConnections.
	void onChannelUp(Channel *channel);
	void onChannelDown(Channel *channel);
	void onMessage(Channel *channel, const Message *message);
	void onException(Channel *channel, const Exception *exception);
	void onTimer(Channel *channel, UInt32 timerID);

	Channel *findChannel(const DatapathID &datapathId);
	
private:
	using DatapathMap = std::map<DatapathID,Channel*>;

    sys::Engine *engine_;
    sys::tcp::endpoint endpt_;
    sys::tcp::acceptor acceptor_;
    sys::tcp::socket socket_;
    ApiConnection *oneConn_ = nullptr;
    DatapathMap datapathMap_;

    ApiServer(Driver *driver, const IPv6Address &localAddress,
              UInt16 localPort);

    void asyncAccept();

    void registerChannel(Channel *channel);
    void unregisterChannel(Channel *channel);
};

} // </namespace yaml>
} // </namespace ofp>

OFP_END_IGNORE_PADDING

#endif // OFP_YAML_APISERVER_H
