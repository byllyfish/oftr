//  ===== ---- ofp/api/apiserver.h -------------------------*- C++ -*- =====  //
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
/// \brief Defines the api::ApiServer class for remote control of OpenFlow
/// driver.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_API_APISERVER_H_
#define OFP_API_APISERVER_H_

#include <map>
#include "ofp/driver.h"
#include "ofp/datapathid.h"

namespace ofp {
namespace api {

class ApiConnection;
class ApiSession;

struct RpcListen;
struct RpcConnect;
struct RpcClose;
struct RpcSend;
struct RpcConfig;
struct RpcListConns;
struct RpcAddIdentity;

OFP_BEGIN_IGNORE_PADDING

/// \brief Implements a server that lets a client control and monitor an
/// OpenFlow driver over stdin/stdout.
/// The driver is controlled using YAML messages.
class ApiServer {
 public:
  ApiServer(Driver *driver, int inputFD, int outputFD,
            Channel *defaultChannel = nullptr);
  ApiServer(Driver *driver, ApiSession *session,
            Channel *defaultChannel = nullptr);

  // Called by ApiConnection to update oneConn_.
  void onConnect(ApiConnection *conn);
  void onDisconnect(ApiConnection *conn);

  void onRpcListen(ApiConnection *conn, RpcListen *open);
  void onRpcConnect(ApiConnection *conn, RpcConnect *connect);
  void onRpcClose(ApiConnection *conn, RpcClose *close);
  void onRpcSend(ApiConnection *conn, RpcSend *send);
  void onRpcConfig(ApiConnection *conn, RpcConfig *config);
  void onRpcListConns(ApiConnection *conn, RpcListConns *list);
  void onRpcAddIdentity(ApiConnection *conn, RpcAddIdentity *add);
  
  // These methods are used to bridge ApiChannelListeners to ApiConnections.
  void onChannelUp(Channel *channel);
  void onChannelDown(Channel *channel);
  void onMessage(Channel *channel, const Message *message);

  Channel *findDatapath(const DatapathID &datapathId, UInt64 connId);

  sys::Engine *engine() { return engine_; }

 private:
  sys::Engine *engine_;
  ApiConnection *oneConn_ = nullptr;
  Channel *defaultChannel_ = nullptr;

  static void connectResponse(ApiConnection *conn, UInt64 id, UInt64 connId, const std::error_code &err);
};

OFP_END_IGNORE_PADDING

}  // namespace api
}  // namespace ofp

#endif  // OFP_API_APISERVER_H_
