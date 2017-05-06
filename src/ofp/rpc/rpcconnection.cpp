// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcconnection.h"
#include "ofp/channel.h"
#include "ofp/rpc/rpcencoder.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using namespace ofp::rpc;

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
  notification.params.type = std::string("CHANNEL_") + status;
  notification.params.time = Timestamp::now();
  notification.params.connId = channel->connectionId();
  notification.params.datapathId = channel->datapathId();
  notification.params.endpoint = channel->remoteEndpoint();
  notification.params.version = channel->version();
  rpcReply(&notification);
}

void RpcConnection::onMessage(Channel *channel, const Message *message) {
  // useJson = true, pktMatch = true
  yaml::Decoder decoder{message, true, true};

  if (decoder.error().empty()) {
    // Send `OFP.MESSAGE` notification event.
    write("{\"params\":");
    write(decoder.result());
    write(",\"method\":\"OFP.MESSAGE\"}");
    write(std::string{RPC_EVENT_DELIMITER_CHAR});  // delimiter

  } else {
    // Send `CHANNEL_ALERT` notification event.
    auto alert = std::string("DECODE FAILED: ") + decoder.error();
    rpcAlert(channel, alert, {message->data(), message->size()}, message->time(), message->xid());

    log_error("OpenFlow parse error:", decoder.error(),
                std::make_pair("connid", message->source()->connectionId()));
  }
}

void RpcConnection::rpcAlert(Channel *channel, const std::string &alert,
                            const ByteRange &data, const Timestamp &time, UInt32 xid) {
  // Send `CHANNEL_ALERT` notification event.
  RpcAlert messageAlert;
  messageAlert.params.type = "CHANNEL_ALERT";
  messageAlert.params.time = time;
  if (channel) {
    messageAlert.params.connId = channel->connectionId();
    messageAlert.params.datapathId = channel->datapathId();
  }
  messageAlert.params.alert = alert;
  messageAlert.params.data = data;
  messageAlert.params.xid = xid;
  rpcReply(&messageAlert);
}

void RpcConnection::handleEvent(const std::string &eventText) {
  RpcEncoder encoder{eventText, this,
                     [this](UInt64 connId, const DatapathID &datapathId) {
                       return server_->findDatapath(connId, datapathId);
                     }};
}

void RpcConnection::rpcRequestTooBig() {
  RpcErrorResponse response{RpcID::NULL_VALUE};
  response.error.code = ERROR_CODE_INVALID_REQUEST;
  response.error.message = "RPC request is too big";
  rpcReply(&response);
}
