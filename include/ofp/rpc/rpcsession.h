// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCSESSION_H_
#define OFP_RPC_RPCSESSION_H_

#include "ofp/rpc/rpcserver.h"

namespace ofp {
namespace rpc {

class RpcConnectionSession;

/// Communicate with an RpcServer running in another thread. Subclass to
/// implement receive() to receive data from the thread (receive() is called
/// from the server thread.)

class RpcSession {
 public:
  RpcSession() : server_{&driver_, this} {}
  virtual ~RpcSession() {}

  void run() { driver_.run(); }
  void stop() { driver_.stop(); }

  void send(const std::string &msg);
  virtual void receive(const std::string &msg) = 0;

  void setConnection(const std::shared_ptr<RpcConnectionSession> &conn);

 private:
  Driver driver_;
  // N.B. conn_ is used in initialization of server_. conn_ also depends on
  // driver_, so it must be initialized after driver_.
  std::shared_ptr<RpcConnectionSession> conn_;
  RpcServer server_;
};

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCSESSION_H_
