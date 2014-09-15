//  ===== ---- ofp/api/apisession.cpp ----------------------*- C++ -*- =====  //
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
/// \brief Defines the api::ApiSession class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/api/apisession.h"
#include "ofp/api/apiconnectionsession.h"
#include "ofp/sys/engine.h"

using ofp::api::ApiSession;

void ApiSession::send(const std::string &msg) {
  asio::io_service &io = driver_.engine()->io();

  auto conn = conn_;
  io.post([conn, msg]() { conn->handleEvent(msg); });  // FIXME - is msg a ref?
}

void ApiSession::setConnection(
    const std::shared_ptr<ApiConnectionSession> &conn) {
  assert(conn_.use_count() == 0);

  conn_ = conn;
}
