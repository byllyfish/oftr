//  ===== ---- ofp/api/apiconnection.h ---------------------*- C++ -*- =====  //
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
/// \brief Defines the api::ApiConnection class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_API_APICONNECTION_H_
#define OFP_API_APICONNECTION_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/api/apiserver.h"
#include "ofp/bytelist.h"

namespace ofp {
namespace api {

struct RpcOpen;
struct RpcOpenResponse;
struct RpcClose;
struct RpcSend;
struct RpcConfig;
struct RpcErrorResponse;

OFP_BEGIN_IGNORE_PADDING

class ApiConnection : public std::enable_shared_from_this<ApiConnection> {
 public:
  explicit ApiConnection(ApiServer *server);
  virtual ~ApiConnection();

  virtual void asyncAccept() = 0;

  void onRpcListen(RpcListen *listen);
  void onRpcConnect(RpcConnect *connect);
  void onRpcClose(RpcClose *close);
  void onRpcSend(RpcSend *send);
  void onRpcConfig(RpcConfig *config);
  void onRpcListConns(RpcListConns *list);
  void onRpcAddIdentity(RpcAddIdentity *add);
  
  template <class Response>
  void rpcReply(Response *response) {
    write(response->toJson());
  }

  void onChannel(Channel *channel, const char *status);
  void onMessage(Channel *channel, const Message *message);

  void handleEvent(const std::string &event);

 protected:
  virtual void write(const std::string &msg) = 0;
  virtual void asyncRead() = 0;

 private:
  ApiServer *server_;
};

OFP_END_IGNORE_PADDING

}  // namespace api
}  // namespace ofp

#endif  // OFP_API_APICONNECTION_H_
