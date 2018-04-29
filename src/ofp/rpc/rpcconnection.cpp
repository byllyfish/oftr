// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcconnection.h"
#include <sys/resource.h>
#include <sys/time.h>
#include "ofp/channel.h"
#include "ofp/rpc/rpcencoder.h"
#include "ofp/sys/engine.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using namespace ofp::rpc;

RpcConnection::RpcConnection(RpcServer *server)
    : server_{server}, metricTimer_{server->engine()->io()} {
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

void RpcConnection::onRpcSetFilter(RpcSetFilter *set) {
  server_->onRpcSetFilter(this, set);
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
    writeEvent(decoder.result(), true);

  } else {
    // Send `CHANNEL_ALERT` notification event.
    auto alert = std::string("DECODE FAILED: ") + decoder.error();
    rpcAlert(channel, alert, {message->data(), message->size()},
             message->time(), message->xid());

    log_error("OpenFlow parse error:", decoder.error(),
              std::make_pair("connid", message->source()->connectionId()));
  }
}

void RpcConnection::rpcAlert(Channel *channel, const std::string &alert,
                             const ByteRange &data, const Timestamp &time,
                             UInt32 xid) {
  DatapathID datapathId;
  UInt64 connId = 0;

  if (channel) {
    connId = channel->connectionId();
    datapathId = channel->datapathId();
  }

  rpcAlert(datapathId, connId, alert, data, time, xid);
}

void RpcConnection::rpcAlert(const DatapathID &datapathId, UInt64 connId,
                             const std::string &alert, const ByteRange &data,
                             const Timestamp &time, UInt32 xid) {
  // Send `CHANNEL_ALERT` notification event.
  RpcAlert messageAlert;
  messageAlert.params.type = "CHANNEL_ALERT";
  messageAlert.params.time = time;
  messageAlert.params.connId = connId;
  messageAlert.params.datapathId = datapathId;
  messageAlert.params.alert = alert;
  messageAlert.params.data = data;
  messageAlert.params.xid = xid;
  rpcReply(&messageAlert);
}

void RpcConnection::handleEvent(const std::string &eventText) {
  ++rxEvents_;
  rxBytes_ += eventText.size() + 1;  // include delimiter char

  RpcEncoder encoder{eventText, this,
                     [this](UInt64 connId, const DatapathID &datapathId) {
                       return server_->findDatapath(connId, datapathId);
                     }};
}

void RpcConnection::rpcRequestInvalid(llvm::StringRef errorMsg) {
  RpcErrorResponse response{RpcID::NULL_VALUE};
  response.error.code = ERROR_CODE_INVALID_REQUEST;
  response.error.message = errorMsg;
  rpcReply(&response);
}

void RpcConnection::asyncMetrics(Milliseconds interval) {
  logMetrics();

  asio::error_code error;
  metricTimer_.expires_after(interval, error);
  assert(!error);

  metricTimer_.async_wait([this, interval](const asio::error_code &err) {
    if (!err) {
      asyncMetrics(interval);
    }
  });
}

void RpcConnection::logMetrics() {
  struct rusage usage;
  ::getrusage(RUSAGE_SELF, &usage);

  Timestamp utime(Unsigned_cast(usage.ru_utime.tv_sec),
                  Unsigned_cast(usage.ru_utime.tv_usec * 1000));
  Timestamp stime(Unsigned_cast(usage.ru_stime.tv_sec),
                  Unsigned_cast(usage.ru_stime.tv_usec * 1000));

// Use task_info for "resident_size"?
#if defined(LIBOFP_TARGET_DARWIN)
  int64_t kbytes = usage.ru_maxrss / 1024;  // convert to kbytes
#else
  int64_t kbytes = usage.ru_maxrss;  // already in kbytes
#endif

  // TODO(bfish): Include SO_NREAD and SO_NWRITE?

  log_info("Metrics", txEvents_, rxEvents_, txBytes_, rxBytes_,
           outgoingBufferSize(), utime, stime, kbytes);
}
