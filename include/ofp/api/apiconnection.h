//  ===== ---- ofp/api/apiconnection.h ---------------------*- C++ -*- =====  //
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
/// \brief Defines the api::ApiConnection class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_API_APICONNECTION_H
#define OFP_API_APICONNECTION_H

#include "ofp/yaml/yllvm.h"
#include "ofp/api/apiserver.h"
#include "ofp/bytelist.h"

OFP_BEGIN_IGNORE_PADDING

namespace ofp { // <namespace ofp>
namespace api { // <namespace api>

struct ApiListenRequest;
struct ApiSetTimer;
struct ApiEditSetting;

class ApiConnection : public std::enable_shared_from_this<ApiConnection> {
public:
	explicit ApiConnection(ApiServer *server, bool listening);
	virtual ~ApiConnection();

	virtual void asyncAccept() = 0;

	void onLoopback(ApiLoopback *loopback);
	void onListenRequest(ApiListenRequest *listenReq);
	void onListenReply(ApiListenReply *listenReply);
	void onConnectRequest(ApiConnectRequest *connectReq);
	void onConnectReply(ApiConnectReply *connectReply);
	void onSetTimer(ApiSetTimer *setTimer);
	void onEditSetting(ApiEditSetting *editSetting);

	void onYamlError(const std::string &error, const std::string &text);
	void onChannelUp(Channel *channel);
	void onChannelDown(Channel *channel);
	void onMessage(Channel *channel, const Message *message);
	void onTimer(Channel *channel, UInt32 timerID);

	void handleEvent(const std::string &event);

protected:
	virtual void write(const std::string &msg) = 0;
	virtual void asyncRead() = 0;

	void processInputLine(std::string *line);

private:
	ApiServer *server_;
	std::string text_;
	bool isListening_ = false;
	bool isFormatJson_ = false;

	static void cleanInputLine(std::string *line);
	static bool isEmptyEvent(const std::string &s);

	enum EventType {
		EmptyEvent,			// empty; too short or no colon
		LibEvent,			// library event
		MsgEvent			// protocol msg event
	};

	static EventType eventTypeOf(const std::string &s);
};

} // </namespace api>
} // </namespace ofp>

OFP_END_IGNORE_PADDING

#endif // OFP_API_APICONNECTION_H
