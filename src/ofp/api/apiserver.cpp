//  ===== ---- ofp/api/apiserver.cpp -----------------------*- C++ -*- =====  //
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

#include "ofp/api/apiserver.h"
#include "ofp/api/apiconnectionstdio.h"
#include "ofp/api/apiconnectionsession.h"
#include "ofp/sys/engine.h"
#include "ofp/api/apichannellistener.h"
#include "ofp/api/apisession.h"
#include "ofp/api/apievents.h"
#include "ofp/api/rpcevents.h"
#include "ofp/sys/connection.h"
#include "ofp/sys/tcp_server.h"

using ofp::api::ApiServer;

ApiServer::ApiServer(Driver *driver, int inputFD, int outputFD,
                     Channel *defaultChannel)
    : engine_{driver->engine()}, defaultChannel_{defaultChannel} {
  // If we're given an existing channel, connect the stdio-based connection
  // directly up to this connection.

  auto conn = std::make_shared<ApiConnectionStdio>(
      this, asio::posix::stream_descriptor{engine_->io()},
      asio::posix::stream_descriptor{engine_->io()});

  if (inputFD >= 0) {
    conn->setInput(inputFD);
  }

  if (outputFD >= 0) {
    conn->setOutput(outputFD);
  }

  conn->asyncAccept();
}

ApiServer::ApiServer(Driver *driver, ApiSession *session,
                     Channel *defaultChannel)
    : engine_{driver->engine()}, defaultChannel_{defaultChannel} {
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

  if (defaultChannel_) {
    engine_->stop(1500_ms);
  } else {
    engine_->stop();
  }

  oneConn_ = nullptr;
}

void ApiServer::onRpcListen(ApiConnection *conn, RpcListen *open) {
  IPv6Endpoint endpt = open->params.endpoint;

  std::error_code err;
  UInt64 connId = engine_->listen(ChannelMode::Controller, endpt, ProtocolVersions::All,
    [this]() { return new ApiChannelListener{this}; }, err);

  if (open->id == RPC_ID_MISSING)
    return;

  if (!err) {
    RpcListenResponse response{open->id};
    response.result.connId = connId;
    conn->rpcReply(&response);

  } else {
    RpcErrorResponse response{open->id};
    response.error.code = err.value();
    response.error.message = err.message();
    conn->rpcReply(&response);
  }
}

void ApiServer::onRpcClose(ApiConnection *conn, RpcClose *close) {

}


void ApiServer::onRpcSend(ApiConnection *conn, RpcSend *send) {
  Channel *channel = findChannel(send->params.datapathId());
  if (channel) {
    channel->write(send->params.data(), send->params.size());
    channel->flush();
  }

  if (send->id == RPC_ID_MISSING)
    return;

  RpcSendResponse response{send->id};
  response.result.data = {send->params.data(), send->params.size()};
  conn->rpcReply(&response);
}


void ApiServer::onRpcListConns(ApiConnection *conn, RpcListConns *list) {
  if (list->id == RPC_ID_MISSING)
    return;

  RpcListConnsResponse response{list->id};
  UInt64 desiredConnId = list->params.connId;

  engine_->forEachServer([desiredConnId, &response](sys::TCP_Server *server){
    UInt64 connId = server->connectionId();
    if (!desiredConnId || connId == desiredConnId) {
      response.result.emplace_back();
      RpcConnectionStats &stats = response.result.back();
      stats.localEndpoint = server->localEndpoint();
      stats.connId = connId;     
    }
  });

  engine_->forEachChannel([desiredConnId, &response](Channel *channel){
    UInt64 connId = channel->connectionId();
    if (!desiredConnId || connId == desiredConnId) {
      response.result.emplace_back();
      RpcConnectionStats &stats = response.result.back();
      stats.localEndpoint = channel->localEndpoint();
      stats.remoteEndpoint = channel->remoteEndpoint();
      stats.connId = connId;
      stats.datapathId = channel->datapathId();
    }
  });

  conn->rpcReply(&response);
}


#if 0
void ApiServer::onConnectRequest(ApiConnection *conn,
                                 ApiConnectRequest *connectReq) {
  Driver *driver = engine_->driver();
  IPv6Endpoint endpt = connectReq->params.endpoint;

  auto deferredErr =
      driver->connect(Driver::Controller, endpt, ProtocolVersions::All,
                      [this]() { return new ApiChannelListener{this}; });

  UInt32 xid = connectReq->params.xid;

  OFP_BEGIN_IGNORE_PADDING

  deferredErr.done([this, endpt, xid](std::error_code err) {
    ApiConnectReply reply;
    reply.params.xid = xid;
    reply.params.endpoint = endpt;
    if (err) {
      reply.params.error = err.message();
    }
    onConnectReply(&reply);
  });

  OFP_END_IGNORE_PADDING
}
#endif //0


void ApiServer::onChannelUp(Channel *channel) {
  if (oneConn_) oneConn_->onChannelUp(channel);
}

void ApiServer::onChannelDown(Channel *channel) {
  if (oneConn_) oneConn_->onChannelDown(channel);
}

void ApiServer::onMessage(Channel *channel, const Message *message) {
  if (oneConn_) oneConn_->onMessage(channel, message);
}

ofp::Channel *ApiServer::findChannel(const DatapathID &datapathId) {
  if (defaultChannel_) return defaultChannel_;

  return engine_->findChannel(datapathId);
}
