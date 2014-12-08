// Copyright 2014-present Bill Fisher. All rights reserved.

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
