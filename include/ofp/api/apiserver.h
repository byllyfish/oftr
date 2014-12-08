// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_API_APISERVER_H_
#define OFP_API_APISERVER_H_

#include <map>
#include "ofp/driver.h"
#include "ofp/datapathid.h"

namespace ofp {
namespace api {

class ApiConnection;
class ApiSession;

struct RpcListen;
struct RpcConnect;
struct RpcClose;
struct RpcSend;
struct RpcListConns;
struct RpcAddIdentity;

OFP_BEGIN_IGNORE_PADDING

/// \brief Implements a server that lets a client control and monitor an
/// OpenFlow driver over stdin/stdout.
/// The driver is controlled using YAML messages.
class ApiServer {
 public:
  ApiServer(Driver *driver, int inputFD, int outputFD,
            Channel *defaultChannel = nullptr);
  ApiServer(Driver *driver, ApiSession *session,
            Channel *defaultChannel = nullptr);

  // Called by ApiConnection to update oneConn_.
  void onConnect(ApiConnection *conn);
  void onDisconnect(ApiConnection *conn);

  void onRpcListen(ApiConnection *conn, RpcListen *open);
  void onRpcConnect(ApiConnection *conn, RpcConnect *connect);
  void onRpcClose(ApiConnection *conn, RpcClose *close);
  void onRpcSend(ApiConnection *conn, RpcSend *send);
  void onRpcListConns(ApiConnection *conn, RpcListConns *list);
  void onRpcAddIdentity(ApiConnection *conn, RpcAddIdentity *add);

  // These methods are used to bridge ApiChannelListeners to ApiConnections.
  void onChannelUp(Channel *channel);
  void onChannelDown(Channel *channel);
  void onMessage(Channel *channel, const Message *message);

  Channel *findDatapath(const DatapathID &datapathId, UInt64 connId);

  sys::Engine *engine() { return engine_; }

 private:
  sys::Engine *engine_;
  ApiConnection *oneConn_ = nullptr;
  Channel *defaultChannel_ = nullptr;

  static void connectResponse(ApiConnection *conn, UInt64 id, UInt64 connId,
                              const std::error_code &err);
};

OFP_END_IGNORE_PADDING

}  // namespace api
}  // namespace ofp

#endif  // OFP_API_APISERVER_H_
