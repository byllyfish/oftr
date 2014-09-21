//  ===== ---- ofp/api/apiconnection.cpp -------------------*- C++ -*- =====  //
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
/// \brief Implements api::ApiConnection class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/api/apiconnection.h"
#include "ofp/api/rpcencoder.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include "ofp/channel.h"

using ofp::api::ApiConnection;

ApiConnection::ApiConnection(ApiServer *server)
    : server_{server} {
  server_->onConnect(this);
}

ApiConnection::~ApiConnection() { 
  server_->onDisconnect(this); 
}

void ApiConnection::onRpcListen(RpcListen *listen) {
  server_->onRpcListen(this, listen);
}

void ApiConnection::onRpcConnect(RpcConnect *connect) {
  server_->onRpcConnect(this, connect);
}

void ApiConnection::onRpcClose(RpcClose *close) {
  server_->onRpcClose(this, close);
}

void ApiConnection::onRpcSend(RpcSend *send) {
  server_->onRpcSend(this, send);
}

void ApiConnection::onRpcConfig(RpcConfig *config) {
  server_->onRpcConfig(this, config);
}

void ApiConnection::onRpcListConns(RpcListConns *list) {
  server_->onRpcListConns(this, list);
}

void ApiConnection::onChannel(Channel *channel, const char *status) {
  RpcDatapath notification;
  notification.params.connId = channel->connectionId();
  notification.params.datapathId = channel->datapathId();
  notification.params.version = channel->version();
  notification.params.status = status;
  rpcReply(&notification);
}

void ApiConnection::onMessage(Channel *channel, const Message *message) {
  yaml::Decoder decoder{RemoveConst_cast(message), true};

  if (decoder.error().empty()) {
    // Send `ofp.message` notification event.
    write("{\"params\":");
    write(decoder.result());
    write(",\"method\":\"ofp.message\"}\n");

  } else {
    // Send `ofp.message_error` notification event.
    RpcMessageError messageError;
    messageError.params.datapathId = channel->datapathId();
    messageError.params.error = decoder.error();
    messageError.params.data = { message->data(), message->size() };
    rpcReply(&messageError);
  }
}

void ApiConnection::handleEvent(const std::string &eventText) {
  RpcEncoder encoder{eventText, this, [this](const DatapathID &datapathId) {
      return server_->findDatapath(datapathId);
    }};
}
