// Copyright (c) 2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCCONNECTIONUNIX_H_
#define OFP_RPC_RPCCONNECTIONUNIX_H_

#include "ofp/rpc/rpcconnection.h"
#include "ofp/sys/asio_utils.h"

namespace ofp {
namespace rpc {

OFP_BEGIN_IGNORE_PADDING

class RpcConnectionUnix final : public RpcConnection {
 public:
  RpcConnectionUnix(RpcServer *server, sys::unix_domain::socket socket,
                    bool binaryProtocol);

  void asyncAccept() override;
  void close() override;

 protected:
  void writeEvent(llvm::StringRef msg, bool ofp_message = false) override;

 private:
  sys::unix_domain::socket sock_;
  asio::streambuf streambuf_;
  Big32 hdrBuf_;
  std::string eventBuf_;

  // Use a two buffer strategy for async-writes. We queue up data in one
  // buffer while we're in the process of writing the other buffer.
  ByteList outgoing_[2];
  int outgoingIdx_ = 0;
  bool writing_ = false;
  bool binaryProtocol_ = false;

  void asyncReadLine();
  void asyncReadHeader();
  void asyncReadMessage(size_t msgLength);

  void asyncWrite();

  size_t outgoingBufferSize() const override {
    return outgoing_[0].size() + outgoing_[1].size();
  }
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCCONNECTIONUNIX_H_
