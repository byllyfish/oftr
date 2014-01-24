//  ===== ---- ofp/yaml/apiserver.cpp ----------------------*- C++ -*- =====  //
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
/// \brief Implements ApiServer class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/yaml/apiserver.h"
#include "ofp/yaml/apiconnectionstdio.h"
#include "ofp/yaml/apiconnectionsession.h"
#include "ofp/sys/engine.h"
#include "ofp/yaml/apichannellistener.h"
#include "ofp/yaml/apisession.h"

using namespace ofp::yaml;
using namespace ofp::sys;

ApiServer::ApiServer(Driver *driver, int inputFD, int outputFD,
                     Channel *defaultChannel)
    : engine_{driver->engine()}, defaultChannel_{defaultChannel} {
  // If we're given an existing channel, connect the stdio-based connection
  // directly up to this connection.

  bool listening = (defaultChannel != nullptr);
  auto conn = std::make_shared<ApiConnectionStdio>(
      this, asio::posix::stream_descriptor{engine_->io()}, asio::posix::stream_descriptor{engine_->io()},
      listening);

  if (inputFD >= 0) {
    conn->setInput(inputFD);
  }

  if (outputFD >= 0) {
    conn->setOutput(outputFD);
  }

  conn->asyncAccept();
}


ApiServer::ApiServer(Driver *driver, ApiSession *session, Channel *defaultChannel)
  : engine_{driver->engine()}, defaultChannel_{defaultChannel}
{
  auto conn = std::make_shared<ApiConnectionSession>(this, session);

  // Give the session a reference to the connection; otherwise, the connection
  // will be deleted when it goes out of scope.

  session->setConnection(conn);
  conn->asyncAccept();
}

void ApiServer::onConnect(ApiConnection *conn) {
  assert(oneConn_ == nullptr);

  log::debug("ApiServer::onConnect");

  oneConn_ = conn;
}

void ApiServer::onDisconnect(ApiConnection *conn) {
  assert(oneConn_ == conn);

  log::debug("ApiServer::onDisconnect");

  // When the one API connection disconnects, shutdown the engine in 1.5 secs.
  // (Only if there are existing channels.)

  if (defaultChannel_ || !datapathMap_.empty()) {
    engine_->stop(1500_ms);
  } else {
    engine_->stop();
  }

  oneConn_ = nullptr;
}

void ApiServer::onListenRequest(ApiConnection *conn,
                                ApiListenRequest *listenReq) {
  Driver *driver = engine_->driver();
  UInt16 listenPort = listenReq->params.listenPort;

  std::error_code err = driver->listen(Driver::Controller,
                            IPv6Endpoint{listenPort}, ProtocolVersions{},
                            [this]() { return new ApiChannelListener{this}; });

  ApiListenReply reply;
  reply.params.listenPort = listenPort;
  if (err) {
    reply.params.error = err.message();
  }
  onListenReply(&reply);
}

void ApiServer::onListenReply(ApiListenReply *listenReply) {
  if (oneConn_) {
    oneConn_->onListenReply(listenReply);
  }
}

void ApiServer::onSetTimer(ApiConnection *conn, ApiSetTimer *setTimer) {
  // FIXME
}

void ApiServer::onChannelUp(Channel *channel) {
  registerChannel(channel);

  if (oneConn_)
    oneConn_->onChannelUp(channel);
}

void ApiServer::onChannelDown(Channel *channel) {
  if (oneConn_)
    oneConn_->onChannelDown(channel);

  unregisterChannel(channel);
}

void ApiServer::onMessage(Channel *channel, const Message *message) {
  if (oneConn_)
    oneConn_->onMessage(channel, message);
}

void ApiServer::onTimer(Channel *channel, UInt32 timerID) {
  if (oneConn_)
    oneConn_->onTimer(channel, timerID);
}

void ApiServer::registerChannel(Channel *channel) {
  assert(datapathMap_.find(channel->datapathId()) == datapathMap_.end());

  datapathMap_[channel->datapathId()] = channel;
}

void ApiServer::unregisterChannel(Channel *channel) {
  datapathMap_.erase(channel->datapathId());
}

ofp::Channel *ApiServer::findChannel(const DatapathID &datapathId) {
  if (defaultChannel_)
    return defaultChannel_;

  auto iter = datapathMap_.find(datapathId);
  if (iter != datapathMap_.end()) {
    return iter->second;
  }

  return nullptr;
}
