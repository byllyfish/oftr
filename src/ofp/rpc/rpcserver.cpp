// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcserver.h"
#include "ofp/rpc/rpcchannellistener.h"
#include "ofp/rpc/rpcconnectionstdio.h"
#include "ofp/rpc/rpcevents.h"
#include "ofp/sys/connection.h"
#include "ofp/sys/engine.h"
#include "ofp/sys/tcp_server.h"
#include "ofp/sys/udp_server.h"

using ofp::rpc::RpcServer;
using ofp::sys::TCP_Server;
using namespace ofp;

RpcServer::RpcServer(int inputFD, int outputFD,
                     Milliseconds metricInterval, Channel *defaultChannel)
    : engine_{driver_.engine()},
      defaultChannel_{defaultChannel},
      metricInterval_{metricInterval} {
  log::fatal_if_false(inputFD >= 0, "inputFD >= 0");
  log::fatal_if_false(outputFD >= 0, "outputFD >= 0");

  // If we're given an existing channel, connect the stdio-based connection
  // directly up to this connection.
  auto conn = std::make_shared<RpcConnectionStdio>(
      this, asio::posix::stream_descriptor{engine_->io(), inputFD},
      asio::posix::stream_descriptor{engine_->io(), outputFD});

  conn->asyncAccept();
  engine_->setAlertCallback(alertCallback, this);

  driver_.installSignalHandlers([this]() { this->close(); });
}

RpcServer::~RpcServer() {
  engine_->setAlertCallback(nullptr, nullptr);
}

void RpcServer::close() {
  log_debug("RpcServer::close");
  if (oneConn_) {
    oneConn_->close();
  }
}

void RpcServer::onConnect(RpcConnection *conn) {
  log_debug("RpcServer::onConnect");
  assert(oneConn_ == nullptr);

  oneConn_ = conn;
}

void RpcServer::onDisconnect(RpcConnection *conn) {
  log_debug("RpcServer::onDisconnect");
  assert(oneConn_ == conn);

  // When the one API connection disconnects, shutdown the engine in 1.5 secs.
  // (Only if there are existing channels.)

  if (defaultChannel_) {
    engine_->stop(1500_ms);
  } else {
    engine_->stop();
  }

  oneConn_ = nullptr;
}

void RpcServer::onRpcListen(RpcConnection *conn, RpcListen *open) {
  IPv6Endpoint endpt = open->params.endpoint;
  UInt64 securityId = open->params.securityId;
  ChannelOptions options = parseOptions(open->params.options);

  // Verify the channel options and that securityId exists.
  if (!verifyOptions(conn, open->id, securityId, options)) {
    // Reply sent if values are invalid.
    return;
  }

  ProtocolVersions versions =
      ProtocolVersions::fromVector(open->params.versions);
  if (versions.empty())
    versions = ProtocolVersions::All;

  std::error_code err;
  UInt64 connId =
      engine_->listen(options, securityId, endpt, versions,
                      [this]() { return new RpcChannelListener{this}; }, err);

  if (open->id.is_missing())
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

void RpcServer::connectResponse(RpcConnection *conn, RpcID id, UInt64 connId,
                                const std::error_code &err) {
  if (id.is_missing())
    return;

  if (!err) {
    RpcConnectResponse response{id};
    response.result.connId = connId;
    conn->rpcReply(&response);
  } else {
    RpcErrorResponse response{id};
    response.error.code = err.value();
    response.error.message = err.message();
    conn->rpcReply(&response);
  }
}

void RpcServer::onRpcConnect(RpcConnection *conn, RpcConnect *connect) {
  std::string optionError;
  UInt64 securityId = connect->params.securityId;
  ChannelOptions options = parseOptions(connect->params.options);

  // Verify the channel options and that securityId exists.
  if (!verifyOptions(conn, connect->id, securityId, options)) {
    // Reply sent if values are invalid.
    return;
  }

  IPv6Endpoint endpt = connect->params.endpoint;
  RpcID id = connect->id;

  ProtocolVersions versions =
      ProtocolVersions::fromVector(connect->params.versions);
  if (versions.empty())
    versions = ProtocolVersions::All;

  auto connPtr = conn->shared_from_this();
  engine_->connect(options, securityId, endpt, versions,
                   [this]() { return new RpcChannelListener{this}; },
                   [connPtr, id](Channel *channel, std::error_code err) {
                     UInt64 connId = channel ? channel->connectionId() : 0;
                     connectResponse(connPtr.get(), id, connId, err);
                   });
}

void RpcServer::onRpcClose(RpcConnection *conn, RpcClose *close) {
  UInt64 connId = close->params.connId;
  DatapathID datapathId = close->params.datapathId;

  if (connId == 0 && datapathId.empty()) {
    // Return an error if no connId or datapathId is specified.
    if (!close->id.is_missing()) {
      RpcErrorResponse response{close->id};
      response.error.code = ERROR_CODE_INVALID_OPTIONS;
      response.error.message = "Invalid options";
      conn->rpcReply(&response);
    }
    return;
  }

  size_t count = engine_->close(connId, datapathId);

  if (close->id.is_missing())
    return;

  RpcCloseResponse response{close->id};
  response.result.count = UInt32_narrow_cast(count);
  conn->rpcReply(&response);
}

void RpcServer::onRpcSend(RpcConnection *conn, RpcSend *send) {
  yaml::Encoder &params = send->params;

  Channel *channel = params.outputChannel();
  if (channel) {
    assert(params.error().empty());
    assert(params.size() > 0);

    // Outgoing channel exists, deliver the message on it.
    channel->write(params.data(), params.size());

    // Flush the message (unless NO_FLUSH flag is specified)
    if (!(params.flags() & OFP_NO_FLUSH) || channel->mustFlush()) {
      channel->flush();
    } else {
      log_debug("onRpcSend: NO_FLUSH",
                std::make_pair("connid", channel->connectionId()));
    }

    // Message delivered successfully to channel. Send optional reply.
    if (!send->id.is_missing()) {
      RpcSendResponse response{send->id};
      response.result.data = {params.data(),
                              std::min<std::size_t>(params.size(), 8)};
      conn->rpcReply(&response);
    }

  } else {
    log_warning("RpcServer:onRpcSend: no outgoing channel");
  }
}

void RpcServer::onRpcListConns(RpcConnection *conn, RpcListConns *list) {
  if (list->id.is_missing())
    return;

  RpcListConnsResponse response{list->id};
  UInt64 desiredConnId = list->params.connId;

  engine_->forEachTCPServer(
      [desiredConnId, &response](sys::TCP_Server *server) {
        UInt64 connId = server->connectionId();
        if (!desiredConnId || connId == desiredConnId) {
          response.result.stats.emplace_back();
          RpcConnectionStats &stats = response.result.stats.back();
          stats.localEndpoint = server->localEndpoint();
          stats.connId = connId;
          stats.auxiliaryId = 0;
          stats.transport = ChannelTransport::TCP_Plaintext;
        }
      });

  engine_->forEachUDPServer(
      [desiredConnId, &response](sys::UDP_Server *server) {
        UInt64 connId = server->connectionId();
        if (!desiredConnId || connId == desiredConnId) {
          response.result.stats.emplace_back();
          RpcConnectionStats &stats = response.result.stats.back();
          stats.localEndpoint = server->localEndpoint();
          stats.connId = connId;
          stats.auxiliaryId = 0;
          stats.transport = ChannelTransport::UDP_Plaintext;
        }
      });

  engine_->forEachConnection([desiredConnId, &response](Channel *channel) {
    UInt64 connId = channel->connectionId();
    if (!desiredConnId || connId == desiredConnId) {
      response.result.stats.emplace_back();
      RpcConnectionStats &stats = response.result.stats.back();
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

void RpcServer::onRpcAddIdentity(RpcConnection *conn, RpcAddIdentity *add) {
  std::error_code err;

#if LIBOFP_ENABLE_OPENSSL
  UInt64 securityId = engine_->addIdentity(
      add->params.cert, add->params.privkey, add->params.cacert,
      add->params.version, add->params.ciphers, err);

  // Nuke security parameters.
  std::memset(&add->params.privkey[0], '\0', add->params.privkey.size());

#else
  UInt64 securityId = 0;
  err = std::make_error_code(std::errc::function_not_supported);
#endif

  if (add->id.is_missing())
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

void RpcServer::onRpcDescription(RpcConnection *conn, RpcDescription *desc) {
  if (desc->id.is_missing())
    return;

  RpcDescriptionResponse response{desc->id};
  response.result.api_version = LIBOFP_RPC_API_VERSION;
  response.result.sw_desc = softwareVersion();
  response.result.versions = ProtocolVersions::All.versions();
  conn->rpcReply(&response);
}

void RpcServer::onChannelUp(Channel *channel) {
  if (oneConn_)
    oneConn_->onChannel(channel, "UP");
}

void RpcServer::onChannelDown(Channel *channel) {
  if (oneConn_)
    oneConn_->onChannel(channel, "DOWN");
}

void RpcServer::onMessage(Channel *channel, const Message *message) {
  if (oneConn_)
    oneConn_->onMessage(channel, message);
}

ofp::Channel *RpcServer::findDatapath(UInt64 connId,
                                      const DatapathID &datapathId) {
  if (defaultChannel_)
    return defaultChannel_;

  return engine_->findDatapath(connId, datapathId);
}

void RpcServer::alertCallback(Channel *channel, const std::string &alert,
                              const ByteRange &data, void *context) {
  RpcServer *self = reinterpret_cast<RpcServer *>(context);
  if (self->oneConn_) {
    self->oneConn_->rpcAlert(channel, alert, data, Timestamp::now());
  }
}

std::string RpcServer::softwareVersion() {
  std::string libofpCommit{LIBOFP_GIT_COMMIT_LIBOFP};
  std::stringstream sstr;

  sstr << LIBOFP_VERSION_STRING << " (" << libofpCommit.substr(0, 7) << ")";

  return sstr.str();
}

ChannelOptions RpcServer::parseOptions(
    const std::vector<std::string> &options) {
  ChannelOptions result = ChannelOptions::NONE;

  for (const auto &opt : options) {
    if (opt == "FEATURES_REQ") {
      result = result | ChannelOptions::FEATURES_REQ;
    } else if (opt == "AUXILIARY") {
      result = result | ChannelOptions::AUXILIARY;
    } else if (opt == "LISTEN_UDP") {
      result = result | ChannelOptions::LISTEN_UDP;
    } else if (opt == "CONNECT_UDP") {
      result = result | ChannelOptions::CONNECT_UDP;
    } else if (opt == "NO_VERSION_CHECK") {
      result = result | ChannelOptions::NO_VERSION_CHECK;
    } else {
      log_warning("RpcServer: Unrecognized option skipped:", opt);
    }
  }

  return result;
}

bool RpcServer::verifyOptions(RpcConnection *conn, RpcID id, UInt64 securityId,
                              ChannelOptions options) {
  // Verify the channel options and that securityId exists.
  std::string errMesg;
  RpcErrorCode errCode = ERROR_CODE_INVALID_REQUEST;
  if (!AreChannelOptionsValid(options)) {
    errMesg = "Invalid combination of options";
  } else if (securityId != 0) {
#if LIBOFP_ENABLE_OPENSSL
    if (engine_->findIdentity(securityId) == nullptr) {
      errMesg = "Invalid tls_id: " + std::to_string(securityId);
    }
#else
    errMesg = "TLS not supported (LIBOFP_ENABLE_OPENSSL=false)";
#endif
  }

  if (!errMesg.empty()) {
    if (!id.is_missing()) {
      RpcErrorResponse response{id};
      response.error.code = errCode;
      response.error.message = errMesg;
      conn->rpcReply(&response);
    }
    return false;
  }

  return true;
}
