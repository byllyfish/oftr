// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_ENGINE_H_
#define OFP_SYS_ENGINE_H_

#include <unordered_map>
#include "ofp/byterange.h"
#include "ofp/channel.h"
#include "ofp/datapathid.h"
#include "ofp/driver.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/defaulthandshake.h"
#include "ofp/sys/saverestore.h"
#include "ofp/sys/tcp_server.h"
#if LIBOFP_ENABLE_OPENSSL
#include "ofp/sys/identity.h"
#endif

namespace ofp {
namespace sys {

OFP_BEGIN_IGNORE_PADDING

class Connection;

class Engine {
 public:
  explicit Engine(Driver *driver);
  ~Engine();

  UInt64 listen(ChannelOptions options, UInt64 securityId,
                const IPv6Endpoint &localEndpoint, ProtocolVersions versions,
                ChannelListener::Factory listenerFactory,
                std::error_code &error);

  UInt64 connect(ChannelOptions options, UInt64 securityId,
                 const IPv6Endpoint &remoteEndpoint, ProtocolVersions versions,
                 ChannelListener::Factory listenerFactory,
                 std::function<void(Channel *, std::error_code)> resultHandler);

  size_t close(UInt64 connId, const DatapathID &dpid);
  size_t closeAll();
  
  void run();
  void stop(Milliseconds timeout = 0_ms);
  bool isRunning() const { return isRunning_; }
  void installSignalHandlers(std::function<void()> callback);

  asio::io_context &io() { return io_; }

  Driver *driver() const { return driver_; }

  bool registerDatapath(Connection *channel);
  void releaseDatapath(Connection *channel);

  UInt64 registerServer(TCP_Server *server);
  void releaseServer(TCP_Server *server);

  UInt64 registerConnection(Connection *connection);
  void releaseConnection(Connection *connection);

  template <class UnaryFunc>
  void forEachConnection(UnaryFunc func) {
    SaveRestore<bool> lock{connListLock_, true};
    std::for_each(connList_.begin(), connList_.end(), func);
  }

  template <class UnaryFunc>
  void forEachTCPServer(UnaryFunc func) {
    SaveRestore<bool> lock{serverListLock_, true};
    std::for_each(serverList_.begin(), serverList_.end(), func);
  }

  template <class UnaryFunc>
  void forEachUDPServer(UnaryFunc func) {
    if (udpConnect_) {
      func(udpConnect_.get());
    }
  }

  template <class UnaryPredicate>
  TCP_Server *findTCPServer(UnaryPredicate func) const {
    SaveRestore<bool> lock{serverListLock_, true};
    auto iter = std::find_if(serverList_.begin(), serverList_.end(), func);
    return iter != serverList_.end() ? *iter : nullptr;
  }

  Connection *findDatapath(UInt64 connId, const DatapathID &dpid) const;

  UInt64 assignConnectionId();

  using AlertCallback = void (*)(Channel *, const std::string &,
                                 const ByteRange &, void *);
  void setAlertCallback(AlertCallback callback, void *context);
  void alert(Channel *conn, const std::string &alert, const ByteRange &data);

#if LIBOFP_ENABLE_OPENSSL
  UInt64 addIdentity(const std::string &certData,
                     const std::string &keyPassphrase,
                     const std::string &verifier, std::error_code &error);

  Identity *findIdentity(UInt64 securityId);
  UInt64 assignSecurityId();
#endif

 private:
  // Pointer to driver object that owns engine.
  Driver *driver_;
  UInt64 lastConnId_ = 0;

// The identities_ vector must be the last object destroyed, since io_service
// objects may depend on it.
#if LIBOFP_ENABLE_OPENSSL
  UInt64 lastSecurityId_ = 0;
  std::vector<std::unique_ptr<Identity>> identities_;
#endif

  std::vector<Connection *> connList_;
  std::vector<TCP_Server *> serverList_;
  std::unordered_map<DatapathID, Connection *> dpidMap_;
  std::shared_ptr<UDP_Server> udpConnect_;

  // The io_context must be one of the first objects to be destroyed when
  // engine destructor runs. Connections may need to update bookkeeping objects.
  asio::io_context io_{1};
  bool isRunning_ = false;

  // Sets up signal handlers to shut down runloop.
  asio::signal_set signals_;

  // Timer that can be used to stop the engine.
  asio::steady_timer stopTimer_;

  // Timer used to poll idle connections.
  asio::steady_timer idleTimer_;

  mutable bool connListLock_ = false;
  mutable bool serverListLock_ = false;

  // Optional function to handle (unusual) alerts, used when there is no other
  // available callback mechanism.
  AlertCallback alertCallback_ = nullptr;
  void *alertContext_ = nullptr;

  UInt64 connectUDP(UInt64 securityId, const IPv6Endpoint &remoteEndpoint,
                    ChannelListener::Factory listenerFactory,
                    std::error_code &error);
  void asyncIdle();

  Connection *findConnId(UInt64 connId) const;
  std::vector<Connection *>::const_iterator findConnIter(UInt64 connId) const;
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_ENGINE_H_
