// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_RPC_RPCCONNECTIONSESSION_H_
#define OFP_RPC_RPCCONNECTIONSESSION_H_

#include "ofp/sys/asio_utils.h"
#include "ofp/rpc/rpcconnection.h"

namespace ofp {
namespace rpc {

OFP_BEGIN_IGNORE_PADDING

class RpcConnectionSession : public RpcConnection {
 public:
  RpcConnectionSession(RpcServer *server, RpcSession *session);

  void asyncAccept() override {}
  void close() override {}

 protected:
  void write(const std::string &msg) override;
  void asyncRead() override;

 private:
  RpcSession *session_;
  asio::io_context::work work_;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCCONNECTIONSESSION_H_
