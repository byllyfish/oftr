// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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

void RpcConnection::onRpcDescription(RpcDescription *desc) {
  server_->onRpcDescription(this, desc);
}

void RpcConnection::onChannel(Channel *channel, const char *status) {
  RpcChannel notification;
  notification.params.connId = channel->connectionId();
  notification.params.datapathId = channel->datapathId();
  notification.params.version = channel->version();
  notification.params.status = status;
  rpcReply(&notification);
}

void RpcConnection::onMessage(Channel *channel, const Message *message) {
  // useJson = true, pktMatch = true
  yaml::Decoder decoder{message, true, true};

  if (decoder.error().empty()) {
    // Send `OFP.MESSAGE` notification event.
    write("{\"params\":");
    write(decoder.result());
    write(",\"method\":\"OFP.MESSAGE\"}\n");

  } else {
    // Send `OFP.ALERT` notification event.
    RpcAlert messageAlert;
    messageAlert.params.connId = channel->connectionId();
    messageAlert.params.datapathId = channel->datapathId();
    messageAlert.params.time = message->time();
    messageAlert.params.alert = "DECODE FAILED: ";
    messageAlert.params.alert += decoder.error();
    messageAlert.params.data = {message->data(), message->size()};
    rpcReply(&messageAlert);

    log::warning("OpenFlow parse error:", decoder.error(),
                 std::make_pair("connid", message->source()->connectionId()));
  }
}

void RpcConnection::onAlert(Channel *channel, const std::string &alert,
                            const ByteRange &data) {
  // Send `OFP.ALERT` notification event.
  RpcAlert messageAlert;
  messageAlert.params.connId = channel->connectionId();
  messageAlert.params.datapathId = channel->datapathId();
  messageAlert.params.alert = alert;
  messageAlert.params.data = data;
  rpcReply(&messageAlert);
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
