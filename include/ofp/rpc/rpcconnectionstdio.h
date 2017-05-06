// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCCONNECTIONSTDIO_H_
#define OFP_RPC_RPCCONNECTIONSTDIO_H_

#include "ofp/rpc/rpcconnection.h"
#include "ofp/sys/asio_utils.h"

namespace ofp {
namespace rpc {

OFP_BEGIN_IGNORE_PADDING

class RpcConnectionStdio final : public RpcConnection {
 public:
  RpcConnectionStdio(RpcServer *server, asio::posix::stream_descriptor input,
                     asio::posix::stream_descriptor output);

  void asyncAccept() override;
  void close() override;

 protected:
  void write(llvm::StringRef msg, bool eom = true) override;
  void asyncRead() override;
  void asyncWrite();

 private:
  asio::posix::stream_descriptor input_;
  asio::posix::stream_descriptor output_;
  asio::streambuf streambuf_;

  // Use a two buffer strategy for async-writes. We queue up data in one
  // buffer while we're in the process of writing the other buffer.
  ByteList outgoing_[2];
  int outgoingIdx_ = 0;
  bool writing_ = false;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCCONNECTIONSTDIO_H_
