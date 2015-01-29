// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_RPC_RPCCHANNELLISTENER_H_
#define OFP_RPC_RPCCHANNELLISTENER_H_

#include "ofp/channellistener.h"

namespace ofp {
namespace rpc {

class RpcServer;

class RpcChannelListener : public ChannelListener {
 public:
  explicit RpcChannelListener(RpcServer *server) : server_{server} {}

  void onChannelUp(Channel *channel) override;
  void onChannelDown(Channel *channel) override;
  void onMessage(const Message *message) override;

 private:
  RpcServer *server_;
  Channel *channel_ = nullptr;
};

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCCHANNELLISTENER_H_
