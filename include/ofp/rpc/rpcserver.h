// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCSERVER_H_
#define OFP_RPC_RPCSERVER_H_

#include <map>
#include "ofp/datapathid.h"
#include "ofp/driver.h"
#include "ofp/rpc/rpcid.h"

namespace ofp {

class ByteRange;

namespace rpc {

class RpcConnection;
class RpcSession;

struct RpcListen;
struct RpcConnect;
struct RpcClose;
struct RpcSend;
struct RpcListConns;
struct RpcAddIdentity;
struct RpcDescription;

OFP_BEGIN_IGNORE_PADDING

/// \brief Implements a server that lets a client control and monitor an
/// OpenFlow driver over stdin/stdout.
/// The driver is controlled using YAML messages.
class RpcServer {
 public:
  RpcServer(Driver *driver, int inputFD, int outputFD, Milliseconds metricInterval,
            Channel *defaultChannel = nullptr);
  ~RpcServer();

  /// Close the control connection.
  void close();

  // Called by RpcConnection to update oneConn_.
  void onConnect(RpcConnection *conn);
  void onDisconnect(RpcConnection *conn);

  void onRpcListen(RpcConnection *conn, RpcListen *open);
  void onRpcConnect(RpcConnection *conn, RpcConnect *connect);
  void onRpcClose(RpcConnection *conn, RpcClose *close);
  void onRpcSend(RpcConnection *conn, RpcSend *send);
  void onRpcListConns(RpcConnection *conn, RpcListConns *list);
  void onRpcAddIdentity(RpcConnection *conn, RpcAddIdentity *add);
  void onRpcDescription(RpcConnection *conn, RpcDescription *desc);

  // These methods are used to bridge RpcChannelListeners to RpcConnections.
  void onChannelUp(Channel *channel);
  void onChannelDown(Channel *channel);
  void onMessage(Channel *channel, const Message *message);

  Channel *findDatapath(UInt64 connId, const DatapathID &datapathId);

  sys::Engine *engine() const { return engine_; }
  Milliseconds metricInterval() const { return metricInterval_; }

 private:
  sys::Engine *engine_;
  RpcConnection *oneConn_ = nullptr;
  Channel *defaultChannel_ = nullptr;
  Milliseconds metricInterval_ = 0_ms;

  static void connectResponse(RpcConnection *conn, RpcID id, UInt64 connId,
                              const std::error_code &err);
  static void alertCallback(Channel *channel, const std::string &alert,
                            const ByteRange &data, void *context);
  static std::string softwareVersion();
  static ChannelOptions parseOptions(const std::vector<std::string> &options);

  bool verifyOptions(RpcConnection *conn, RpcID id, UInt64 securityId,
                     ChannelOptions options);
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCSERVER_H_
