// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/rpc/rpcsession.h"
#include "ofp/rpc/rpcconnectionsession.h"
#include "ofp/sys/engine.h"

using ofp::rpc::RpcSession;

void RpcSession::send(const std::string &msg) {
  asio::io_service &io = driver_.engine()->io();

  auto conn = conn_;
  io.post([conn, msg]() { conn->handleEvent(msg); });
}

void RpcSession::setConnection(
    const std::shared_ptr<RpcConnectionSession> &conn) {
  assert(conn_.use_count() == 0);

  conn_ = conn;
}
