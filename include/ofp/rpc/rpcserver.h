// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCSERVER_H_
#define OFP_RPC_RPCSERVER_H_

#include <map>
#include "ofp/datapathid.h"
#include "ofp/driver.h"
#include "ofp/rpc/filtertable.h"
#include "ofp/rpc/rpcid.h"
#include "ofp/sys/asio_utils.h"

namespace ofp {

class ByteRange;

namespace rpc {

class RpcConnection;

struct RpcListen;
struct RpcConnect;
struct RpcClose;
struct RpcSend;
struct RpcListConns;
struct RpcAddIdentity;
struct RpcDescription;
struct RpcSetFilter;

OFP_BEGIN_IGNORE_PADDING

/// \brief Implements a server that lets a client control and monitor an
/// OpenFlow driver over stdin/stdout.
/// The driver is controlled using YAML messages.
class RpcServer {
 public:
  RpcServer(bool binaryProtocol, Milliseconds metricInterval,
            Channel *defaultChannel = nullptr);
  ~RpcServer();

  /// Bind RPC server to stdio or unix domain socket.
  std::error_code bind(int inputFD, int outputFD);
  std::error_code bind(int socketFD);
  std::error_code bind(const std::string &listenPath);

  /// Run the rpc server.
  void run() { driver_.run(); }

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
  void onRpcSetFilter(RpcConnection *conn, RpcSetFilter *set);

  // These methods are used to bridge RpcChannelListeners to RpcConnections.
  void onChannelUp(Channel *channel);
  void onChannelDown(Channel *channel);
  void onMessage(Channel *channel, Message *message);

  Channel *findDatapath(UInt64 connId, const DatapathID &datapathId);

  sys::Engine *engine() const { return engine_; }
  Milliseconds metricInterval() const { return metricInterval_; }

 private:
  Driver driver_;
  sys::Engine *engine_;
  sys::unix_domain::acceptor acceptor_;
  sys::unix_domain::socket socket_;
  bool binaryProtocol_ = false;
  RpcConnection *oneConn_ = nullptr;
  Channel *defaultChannel_ = nullptr;
  Milliseconds metricInterval_ = 0_ms;
  FilterTable filter_;

  void asyncAccept();

  static void connectResponse(RpcConnection *conn, RpcID id, UInt64 connId,
                              const std::error_code &err);
  static void alertCallback(Channel *channel, const std::string &alert,
                            const ByteRange &data, void *context);
  static std::string softwareVersion();
  static ChannelOptions parseOptions(const std::vector<std::string> &options);

  bool verifyOptions(RpcConnection *conn, RpcID id, UInt64 securityId,
                     ChannelOptions options);

  std::error_code deleteExistingSocketFile(const std::string &listenPath);
  std::error_code verifySocketFD(int socketFD) const;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCSERVER_H_
