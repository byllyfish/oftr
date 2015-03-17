// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/rpc/rpcconnection.h"
#include "ofp/rpc/rpcencoder.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include "ofp/channel.h"

using ofp::rpc::RpcConnection;

RpcConnection::RpcConnection(RpcServer *server) : server_{server} {
  server_->onConnect(this);
}

RpcConnection::~RpcConnection() {
  server_->onDisconnect(this);
}

void RpcConnection::onRpcListen(RpcListen *listen) {
  server_->onRpcListen(this, listen);
}

void RpcConnection::onRpcConnect(RpcConnect *connect) {
  server_->onRpcConnect(this, connect);
}

void RpcConnection::onRpcClose(RpcClose *close) {
  server_->onRpcClose(this, close);
}

void RpcConnection::onRpcSend(RpcSend *send) {
  server_->onRpcSend(this, send);
}

void RpcConnection::onRpcListConns(RpcListConns *list) {
  server_->onRpcListConns(this, list);
}

void RpcConnection::onRpcAddIdentity(RpcAddIdentity *add) {
  server_->onRpcAddIdentity(this, add);
}

void RpcConnection::onChannel(Channel *channel, const char *status) {
  RpcDatapath notification;
  notification.params.connId = channel->connectionId();
  notification.params.datapathId = channel->datapathId();
  notification.params.version = channel->version();
  notification.params.status = status;
  rpcReply(&notification);
}

void RpcConnection::onMessage(Channel *channel, const Message *message) {
  yaml::Decoder decoder{message, true};

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

    log::warning("OpenFlow parse error:", decoder.error(),
                 std::make_pair("connid", message->source()->connectionId()));
  }
}

void RpcConnection::handleEvent(const std::string &eventText) {
  RpcEncoder encoder{eventText, this,
                     [this](const DatapathID &datapathId, UInt64 connId) {
                       return server_->findDatapath(datapathId, connId);
                     }};
}


void RpcConnection::rpcRequestTooBig() {
  RpcErrorResponse response{RPC_ID_MISSING};
  response.error.code = ERROR_CODE_INVALID_REQUEST;
  response.error.message = "RPC request is too big";
  rpcReply(&response);
}
