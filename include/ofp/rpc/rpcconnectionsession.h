// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCCONNECTIONSESSION_H_
#define OFP_RPC_RPCCONNECTIONSESSION_H_

#include "ofp/rpc/rpcconnection.h"
#include "ofp/sys/asio_utils.h"

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
  asio::executor_work_guard<asio::io_context::executor_type> work_;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCCONNECTIONSESSION_H_
