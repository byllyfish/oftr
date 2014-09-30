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
#include "ofp/sys/udp_server.h"

using ofp::api::ApiServer;
using ofp::sys::TCP_Server;
using ofp::UInt32;

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
  UInt64 securityId = 0;

  std::error_code err;
  UInt64 connId = engine_->listen(ChannelMode::Controller, securityId, endpt, ProtocolVersions::All,
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

void ApiServer::onRpcConnect(ApiConnection *conn, RpcConnect *connect) {
  std::string optionError;
  ChannelMode mode = ChannelMode::Controller;
  ChannelTransport transport = ChannelTransport::TCP_Plaintext;
  UInt64 securityId = 0;

  for (auto opt : connect->params.options) {
    if (opt == "--raw" || opt == "-raw") {
      mode = ChannelMode::Raw;
    } else if (opt == "--udp" || opt == "-udp") {
      transport = ChannelTransport::UDP_Plaintext;
    } else {
      optionError += "Unknown option: " + opt;
      break;
    }
  }

  // Check for invalid combination of mode and transport.
  if (IsChannelTransportUDP(transport) && mode != ChannelMode::Raw) {
    optionError += "Invalid option: --udp must be combined with --raw";
  }

  if (!optionError.empty()) {
    if (connect->id == RPC_ID_MISSING)
      return;
    RpcErrorResponse response{connect->id};
    response.error.code = ERROR_CODE_INVALID_OPTION;
    response.error.message = optionError;
    conn->rpcReply(&response);
    return;
  }

  IPv6Endpoint endpt = connect->params.endpoint;
  UInt64 id = connect->id;
  auto connPtr = conn->shared_from_this();

  engine_->connect(mode, transport, securityId, endpt, ProtocolVersions::All, 
    [this]() { return new ApiChannelListener{this}; }, 
    [connPtr, id](Channel *channel, std::error_code err) {
      if (id == RPC_ID_MISSING) 
        return;
      if (!err) {
        RpcConnectResponse response{id};
        response.result.connId = channel->connectionId();
        connPtr->rpcReply(&response);
      } else {
        RpcErrorResponse response{id};
        response.error.code = err.value();
        response.error.message = err.message();
        connPtr->rpcReply(&response);        
      }
    });
}

void ApiServer::onRpcClose(ApiConnection *conn, RpcClose *close) {
  size_t count = engine_->close(close->params.connId);

  if (close->id == RPC_ID_MISSING)
    return;

  RpcCloseResponse response{close->id};
  response.result.count = UInt32_narrow_cast(count);
  conn->rpcReply(&response);
}


void ApiServer::onRpcSend(ApiConnection *conn, RpcSend *send) {
  UInt64 connId = 0;
  yaml::Encoder &params = send->params;

  Channel *channel = params.outputChannel();
  if (channel) {
    connId = channel->connectionId();
    channel->write(params.data(), params.size());
    channel->flush();
  }

  if (send->id == RPC_ID_MISSING)
    return;

  RpcSendResponse response{send->id};
  response.result.connId = connId;
  response.result.data = {params.data(), params.size()};
  conn->rpcReply(&response);
}

void ApiServer::onRpcConfig(ApiConnection *conn, RpcConfig *config) {
  // FIXME - to be implemented.
}

void ApiServer::onRpcListConns(ApiConnection *conn, RpcListConns *list) {
  if (list->id == RPC_ID_MISSING)
    return;

  RpcListConnsResponse response{list->id};
  UInt64 desiredConnId = list->params.connId;

  engine_->forEachTCPServer([desiredConnId, &response](sys::TCP_Server *server){
    UInt64 connId = server->connectionId();
    if (!desiredConnId || connId == desiredConnId) {
      response.result.emplace_back();
      RpcConnectionStats &stats = response.result.back();
      stats.localEndpoint = server->localEndpoint();
      stats.connId = connId;
      stats.auxiliaryId = 0;
      stats.transport = ChannelTransport::TCP_Plaintext;
    }
  });

  engine_->forEachUDPServer([desiredConnId, &response](sys::UDP_Server *server){
    UInt64 connId = server->connectionId();
    if (!desiredConnId || connId == desiredConnId) {
      response.result.emplace_back();
      RpcConnectionStats &stats = response.result.back();
      stats.localEndpoint = server->localEndpoint();
      stats.connId = connId;
      stats.auxiliaryId = 0;
      stats.transport = ChannelTransport::UDP_Plaintext;
    }
  });

  engine_->forEachConnection([desiredConnId, &response](Channel *channel){
    UInt64 connId = channel->connectionId();
    if (!desiredConnId || connId == desiredConnId) {
      response.result.emplace_back();
      RpcConnectionStats &stats = response.result.back();
      stats.localEndpoint = channel->localEndpoint();
      stats.remoteEndpoint = channel->remoteEndpoint();
      stats.connId = connId;
      stats.datapathId = channel->datapathId();
      stats.auxiliaryId = channel->auxiliaryId();
      stats.transport = channel->transport();
    }
  });

  conn->rpcReply(&response);
}

void ApiServer::onRpcAddIdentity(ApiConnection *conn, RpcAddIdentity *add) {
  std::error_code err;
  UInt64 securityId = engine_->addIdentity(add->params.certificate, err);

  if (add->id == RPC_ID_MISSING)
    return;

  if (!err) {
    RpcAddIdentityResponse response{add->id};
    response.result.securityId = securityId;
    conn->rpcReply(&response);
  } else {
    RpcErrorResponse response{add->id};
    response.error.code = err.value();
    response.error.message = err.message();
    conn->rpcReply(&response);    
  }
}

void ApiServer::onChannelUp(Channel *channel) {
  if (oneConn_) oneConn_->onChannel(channel, "UP");
}

void ApiServer::onChannelDown(Channel *channel) {
  if (oneConn_) oneConn_->onChannel(channel, "DOWN");
}

void ApiServer::onMessage(Channel *channel, const Message *message) {
  if (oneConn_) oneConn_->onMessage(channel, message);
}

ofp::Channel *ApiServer::findDatapath(const DatapathID &datapathId, UInt64 connId) {
  if (defaultChannel_) return defaultChannel_;

  return engine_->findDatapath(datapathId, connId);
}
