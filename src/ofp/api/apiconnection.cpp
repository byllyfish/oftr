// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/api/apiconnection.h"
#include "ofp/api/rpcencoder.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include "ofp/channel.h"

using ofp::api::ApiConnection;

ApiConnection::ApiConnection(ApiServer *server) : server_{server} {
  server_->onConnect(this);
}

ApiConnection::~ApiConnection() { server_->onDisconnect(this); }

void ApiConnection::onRpcListen(RpcListen *listen) {
  server_->onRpcListen(this, listen);
}

void ApiConnection::onRpcConnect(RpcConnect *connect) {
  server_->onRpcConnect(this, connect);
}

void ApiConnection::onRpcClose(RpcClose *close) {
  server_->onRpcClose(this, close);
}

void ApiConnection::onRpcSend(RpcSend *send) { server_->onRpcSend(this, send); }

void ApiConnection::onRpcConfig(RpcConfig *config) {
  server_->onRpcConfig(this, config);
}

void ApiConnection::onRpcListConns(RpcListConns *list) {
  server_->onRpcListConns(this, list);
}

void ApiConnection::onRpcAddIdentity(RpcAddIdentity *add) {
  server_->onRpcAddIdentity(this, add);
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
    messageError.params.data = {message->data(), message->size()};
    rpcReply(&messageError);

    log::warning("OpenFlow parse error:", decoder.error(), std::make_pair("connid", message->source()->connectionId()));
  }
}

void ApiConnection::handleEvent(const std::string &eventText) {
  RpcEncoder encoder{eventText, this,
                     [this](const DatapathID &datapathId, UInt64 connId) {
    return server_->findDatapath(datapathId, connId);
  }};
}
