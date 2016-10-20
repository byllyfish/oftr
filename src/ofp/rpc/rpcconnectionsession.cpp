// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcconnectionsession.h"
#include "ofp/rpc/rpcsession.h"
#include "ofp/sys/engine.h"

using ofp::rpc::RpcConnectionSession;

RpcConnectionSession::RpcConnectionSession(RpcServer *server,
                                           RpcSession *session)
    : RpcConnection{server}, session_{session}, work_{asio::make_work_guard(server->engine()->io())} {}

void RpcConnectionSession::write(const std::string &msg) {
  session_->receive(msg);
}

void RpcConnectionSession::asyncRead() {}
