//  ===== ---- ofp/yaml/apiconnection.h --------------------*- C++ -*- =====  //
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
/// \brief Defines the yaml::ApiConnection class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_APICONNECTION_H
#define OFP_YAML_APICONNECTION_H

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/apiserver.h"
#include "ofp/bytelist.h"

OFP_BEGIN_IGNORE_PADDING

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

struct ApiListenRequest;
struct ApiSetTimer;

class ApiConnection : public std::enable_shared_from_this<ApiConnection> {
public:
	ApiConnection(ApiServer *server);
	virtual ~ApiConnection();

	virtual void asyncAccept() = 0;
	
	void onLoopback(ApiLoopback *loopback);
	void onListenRequest(ApiListenRequest *listenReq);
	void onListenReply(ApiListenReply *listenReply);
	void onSetTimer(ApiSetTimer *setTimer);

	void onYamlError(const std::string &error, const std::string &text);
	void onChannelUp(Channel *channel);
	void onChannelDown(Channel *channel);
	void onMessage(Channel *channel, const Message *message);
	void onException(Channel *channel, const Exception *exception);
	void onTimer(Channel *channel, UInt32 timerID);

protected:
	virtual void write(const std::string &msg) = 0;
	virtual void asyncRead() = 0;
	virtual void asyncWrite() = 0;

	void handleInputLine(std::string *line);

private:
	ApiServer *server_;
	//sys::tcp::socket socket_;
	//boost::asio::streambuf streambuf_;
	std::string text_;
	unsigned lineCount_ = 0;
	bool isLibEvent_ = false;
	bool isListening_ = false;

	// Use a two buffer strategy for async-writes. We queue up data in one
	// buffer while we're in the process of writing the other buffer.
	//ByteList outgoing_[2];
	//int outgoingIdx_ = 0;
	//bool writing_ = false;
	
	void handleEvent();

	static void cleanInputLine(std::string *line);
	static bool isEmptyEvent(const std::string &s);
	static std::string escape(const std::string &s);
};

} // </namespace yaml>
} // </namespace ofp>

OFP_END_IGNORE_PADDING

#endif // OFP_YAML_APICONNECTION_H
