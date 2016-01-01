// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/rpc/rpcserver.h"
#include "ofp/rpc/rpcconnectionstdio.h"
#include "ofp/rpc/rpcconnectionsession.h"
#include "ofp/sys/engine.h"
#include "ofp/rpc/rpcchannellistener.h"
#include "ofp/rpc/rpcsession.h"
#include "ofp/rpc/rpcevents.h"
#include "ofp/sys/connection.h"
#include "ofp/sys/tcp_server.h"
#include "ofp/sys/udp_server.h"

using ofp::rpc::RpcServer;
using ofp::sys::TCP_Server;
using ofp::UInt32;

RpcServer::RpcServer(Driver *driver, int inputFD, int outputFD,
                     Channel *defaultChannel)
    : engine_{driver->engine()}, defaultChannel_{defaultChannel} {
  // If we're given an existing channel, connect the stdio-based connection
  // directly up to this connection.

  auto conn = std::make_shared<RpcConnectionStdio>(
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

RpcServer::RpcServer(Driver *driver, RpcSession *session,
                     Channel *defaultChannel)
    : engine_{driver->engine()}, defaultChannel_{defaultChannel} {
  auto conn = std::make_shared<RpcConnectionSession>(this, session);

  // Give the session a reference to the connection; otherwise, the connection
  // will be deleted when it goes out of scope.

  session->setConnection(conn);
  conn->asyncAccept();
}

void RpcServer::close() {
  log::debug("RpcServer::close");
  if (oneConn_) {
    oneConn_->close();
  }
}

void RpcServer::onConnect(RpcConnection *conn) {
  log::debug("RpcServer::onConnect");
  assert(oneConn_ == nullptr);

  oneConn_ = conn;
}

void RpcServer::onDisconnect(RpcConnection *conn) {
  log::debug("RpcServer::onDisconnect");
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
  std::string optionError;
  IPv6Endpoint endpt = open->params.endpoint;
  UInt64 securityId = open->params.securityId;

  // Check that securityId exists.
  if (securityId != 0 && engine_->findIdentity(securityId) == nullptr) {
    optionError += "Invalid tls_id: " + std::to_string(securityId);
  }

  if (!optionError.empty()) {
    if (open->id == RPC_ID_MISSING)
      return;
    RpcErrorResponse response{open->id};
    response.error.code = ERROR_CODE_INVALID_OPTION;
    response.error.message = optionError;
    conn->rpcReply(&response);
    return;
  }

  ProtocolVersions versions =
      ProtocolVersions::fromVector(open->params.versions);
  if (versions.empty())
    versions = ProtocolVersions::All;

  std::error_code err;
  UInt64 connId =
      engine_->listen(ChannelMode::Controller, securityId, endpt, versions,
                      [this]() { return new RpcChannelListener{this}; }, err);

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

void RpcServer::connectResponse(RpcConnection *conn, UInt64 id, UInt64 connId,
                                const std::error_code &err) {
  if (id == RPC_ID_MISSING)
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
  ChannelMode mode = ChannelMode::Controller;
  UInt64 securityId = connect->params.securityId;
  bool udp = false;

  for (auto opt : connect->params.options) {
    if (opt == "--raw" || opt == "-raw") {
      mode = ChannelMode::Raw;
    } else if (opt == "--udp" || opt == "-udp") {
      udp = true;
    } else {
      optionError += "Unknown option: " + opt;
      break;
    }
  }

  // Check for invalid combination of mode and transport.
  if (udp && mode != ChannelMode::Raw) {
    optionError += "Invalid option: --udp must be combined with --raw";
  }

  // Check that securityId exists.
  if (optionError.empty() && securityId != 0 &&
      engine_->findIdentity(securityId) == nullptr) {
    optionError += "Invalid tls_id: " + std::to_string(securityId);
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

  ProtocolVersions versions =
      ProtocolVersions::fromVector(connect->params.versions);
  if (versions.empty())
    versions = ProtocolVersions::All;

  if (udp) {
    std::error_code err;
    UInt64 connId = engine_->connectUDP(
        mode, securityId, endpt, versions,
        [this]() { return new RpcChannelListener{this}; }, err);
    connectResponse(conn, id, connId, err);

  } else {
    auto connPtr = conn->shared_from_this();
    engine_->connect(mode, securityId, endpt, versions,
                     [this]() { return new RpcChannelListener{this}; },
                     [connPtr, id](Channel *channel, std::error_code err) {
                       UInt64 connId = channel ? channel->connectionId() : 0;
                       connectResponse(connPtr.get(), id, connId, err);
                     });
  }
}

void RpcServer::onRpcClose(RpcConnection *conn, RpcClose *close) {
  size_t count = engine_->close(close->params.connId);

  if (close->id == RPC_ID_MISSING)
    return;

  RpcCloseResponse response{close->id};
  response.result.count = UInt32_narrow_cast(count);
  conn->rpcReply(&response);
}

void RpcServer::onRpcSend(RpcConnection *conn, RpcSend *send) {
  UInt64 connId = 0;
  yaml::Encoder &params = send->params;

  Channel *channel = params.outputChannel();
  if (channel) {
    connId = channel->connectionId();
    channel->write(params.data(), params.size());
    channel->flush();
  } else {
    log::error("onRpcSend unable to locate output channel");
  }

  if (send->id == RPC_ID_MISSING)
    return;

  RpcSendResponse response{send->id};
  response.result.connId = connId;
  response.result.data = {params.data(), params.size()};
  conn->rpcReply(&response);
}

void RpcServer::onRpcListConns(RpcConnection *conn, RpcListConns *list) {
  if (list->id == RPC_ID_MISSING)
    return;

  RpcListConnsResponse response{list->id};
  UInt64 desiredConnId = list->params.connId;

  engine_->forEachTCPServer(
      [desiredConnId, &response](sys::TCP_Server *server) {
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

  engine_->forEachUDPServer(
      [desiredConnId, &response](sys::UDP_Server *server) {
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

  engine_->forEachConnection([desiredConnId, &response](Channel *channel) {
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

void RpcServer::onRpcAddIdentity(RpcConnection *conn, RpcAddIdentity *add) {
  std::error_code err;
  UInt64 securityId = engine_->addIdentity(
      add->params.certificate, add->params.password, add->params.verifier, err);

  // add->params.password.fill('x');

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

void RpcServer::onRpcDescription(RpcConnection *conn, RpcDescription *desc) {
  if (desc->id == RPC_ID_MISSING)
    return;

  RpcDescriptionResponse response{desc->id};
  response.result.major_version = LIBOFP_RPC_API_MAJOR;
  response.result.minor_version = LIBOFP_RPC_API_MINOR;
  response.result.software_version = softwareVersion();
  response.result.ofp_versions = ProtocolVersions::All.versions();
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

ofp::Channel *RpcServer::findDatapath(const DatapathID &datapathId,
                                      UInt64 connId) {
  if (defaultChannel_)
    return defaultChannel_;

  return engine_->findDatapath(datapathId, connId);
}

std::string RpcServer::softwareVersion() {
  std::string libofpCommit{LIBOFP_GIT_COMMIT_LIBOFP};
  std::stringstream sstr;

  sstr << LIBOFP_VERSION_STRING << " (" << libofpCommit.substr(0, 7) << ")";

  return sstr.str();
}
