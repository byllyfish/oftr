// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCCONNECTION_H_
#define OFP_RPC_RPCCONNECTION_H_

#include "ofp/bytelist.h"
#include "ofp/rpc/rpcserver.h"
#include "ofp/timestamp.h"
#include "ofp/yaml/yllvm.h"

namespace ofp {
namespace rpc {

struct RpcOpen;
struct RpcOpenResponse;
struct RpcClose;
struct RpcSend;
struct RpcErrorResponse;

OFP_BEGIN_IGNORE_PADDING

class RpcConnection : public std::enable_shared_from_this<RpcConnection> {
 public:
  explicit RpcConnection(RpcServer *server);
  virtual ~RpcConnection();

  virtual void asyncAccept() = 0;
  virtual void close() = 0;

  void onRpcListen(RpcListen *listen);
  void onRpcConnect(RpcConnect *connect);
  void onRpcClose(RpcClose *close);
  void onRpcSend(RpcSend *send);
  void onRpcListConns(RpcListConns *list);
  void onRpcAddIdentity(RpcAddIdentity *add);
  void onRpcDescription(RpcDescription *desc);

  template <class Response>
  void rpcReply(Response *response) {
    write(response->toJson());
  }

  void onChannel(Channel *channel, const char *status);
  void onMessage(Channel *channel, const Message *message);

  void rpcAlert(Channel *channel, const std::string &alert,
                const ByteRange &data, const Timestamp &time, UInt32 xid = 0);
  void rpcAlert(const DatapathID &datapathId, UInt64 connId,
                const std::string &alert, const ByteRange &data,
                const Timestamp &time, UInt32 xid = 0);

  void handleEvent(const std::string &eventText);

 protected:
  RpcServer *server_;
  UInt32 txEvents_ = 0;
  UInt32 rxEvents_ = 0;
  UInt64 txBytes_ = 0;
  UInt64 rxBytes_ = 0;

  virtual void write(llvm::StringRef msg, bool eom = true) = 0;
  virtual void asyncRead() = 0;

  void rpcRequestTooBig();
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCCONNECTION_H_
