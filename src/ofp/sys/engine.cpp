// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/engine.h"
#include "ofp/log.h"
#include "ofp/sys/tcp_connection.h"
#include "ofp/sys/tcp_server.h"
#include "ofp/sys/udp_server.h"

using namespace ofp;
using namespace ofp::sys;

Engine::Engine(Driver *driver)
    : driver_{driver}, signals_{io_}, stopTimer_{io_}, idleTimer_{io_} {
  log_debug("Engine ready");
}

Engine::~Engine() {
  // Shutdown all existing connections and servers.
  (void)closeAll();
  log_debug("Engine shutting down");
}

UInt64 Engine::listen(ChannelOptions options, UInt64 securityId,
                      const IPv6Endpoint &localEndpoint,
                      ProtocolVersions versions,
                      ChannelListener::Factory listenerFactory,
                      std::error_code &error) {
  // Verify the channel options.
  if (!AreChannelOptionsValid(options)) {
    error = std::make_error_code(std::errc::invalid_argument);
    return 0;
  }

  auto svrPtr = TCP_Server::create(this, options, securityId, localEndpoint,
                                   versions, listenerFactory, error);
  if (error)
    return 0;

  // If there's no error, the TCP_Server has registered itself with the engine.
  UInt64 connId = svrPtr->connectionId();
  assert(connId > 0);

  return connId;
}

UInt64 Engine::connect(
    ChannelOptions options, UInt64 securityId,
    const IPv6Endpoint &remoteEndpoint, ProtocolVersions versions,
    ChannelListener::Factory listenerFactory,
    std::function<void(Channel *, std::error_code)> resultHandler) {
  // Verify the channel options.
  if (!AreChannelOptionsValid(options)) {
    resultHandler(nullptr, std::make_error_code(std::errc::invalid_argument));
    return 0;
  }

  UInt64 connId = 0;

  // Check for CONNECT_UDP option.
  if ((options & ChannelOptions::CONNECT_UDP) != 0) {
    std::error_code error;
    connId = connectUDP(securityId, remoteEndpoint, listenerFactory, error);
    resultHandler(nullptr, error);
    return connId;
  }

  if (securityId != 0) {
    connId = TCP_AsyncConnect<EncryptedSocket>(this, options, securityId,
                                               versions, listenerFactory,
                                               remoteEndpoint, resultHandler);

  } else {
    connId = TCP_AsyncConnect<PlaintextSocket>(this, options, securityId,
                                               versions, listenerFactory,
                                               remoteEndpoint, resultHandler);
  }

  return connId;
}

UInt64 Engine::connectUDP(UInt64 securityId, const IPv6Endpoint &remoteEndpoint,
                          ChannelListener::Factory listenerFactory,
                          std::error_code &error) {
  UInt64 connId = 0;

  if (!udpConnect_) {
    udpConnect_ = UDP_Server::create(this, error);
    if (error) {
      udpConnect_.reset();
    }
  }

  if (udpConnect_) {
    connId = udpConnect_->connect(remoteEndpoint, securityId, listenerFactory,
                                  error);
  }

  return connId;
}

// Close a specific server or connection.
size_t Engine::close(UInt64 connId, const DatapathID &dpid) {
  if (dpid.empty()) {
    if (connId == 0) {
      return 0;
    }

    TCP_Server *server = findTCPServer(
        [connId](TCP_Server *svr) { return svr->connectionId() == connId; });

    if (server) {
      server->shutdown();
      return 1;
    }
  }

  Connection *conn = findDatapath(connId, dpid);
  if (conn) {
    conn->shutdown();
    if (conn->flags() & Connection::kManualDelete) {
      delete conn;
    }
    return 1;
  }

  return 0;
}

size_t Engine::closeAll() {
  // Close all servers and connections.
  size_t result = serverList_.size() + connList_.size() + (udpConnect_ ? 1 : 0);
  if (result == 0)
    return 0;

  log_info("Close all servers and connections");

  std::vector<TCP_Server *> servers;
  servers.swap(serverList_);
  for (auto svr : servers) {
    svr->shutdown();
  }

  std::vector<Connection *> conns;
  conns.swap(connList_);
  for (auto conn : conns) {
    conn->shutdown();
    if (conn->flags() & Connection::kManualDelete) {
      delete conn;
    }
  }

  if (udpConnect_) {
    udpConnect_->shutdown();
    udpConnect_.reset();
  }

  return result;
}

#if LIBOFP_ENABLE_OPENSSL

UInt64 Engine::addIdentity(const std::string &certData,
                           const std::string &privKey,
                           const std::string &verifier,
                           const std::string &version,
                           const std::string &ciphers,
                           const std::string &keyLogFile,
                           std::error_code &error) {
  auto idPtr = MakeUniquePtr<Identity>(certData, privKey, verifier, version,
                                       ciphers, keyLogFile, error);
  if (error)
    return 0;

  UInt64 secId = assignSecurityId();
  assert(secId > 0);
  idPtr->setSecurityId(secId);

  log_info("Add TLS identity:", idPtr->subjectName(),
           std::make_pair("tlsid", secId));

  identities_.push_back(std::move(idPtr));

  return secId;
}

Identity *Engine::findIdentity(UInt64 securityId) {
  if (securityId == 0) {
    return nullptr;
  }

  auto iter = std::find_if(identities_.begin(), identities_.end(),
                           [securityId](std::unique_ptr<Identity> &identity) {
                             return identity->securityId() == securityId;
                           });

  return iter != identities_.end() ? iter->get() : nullptr;
}

UInt64 Engine::assignSecurityId() {
  if (++lastSecurityId_ == 0) {
    static_assert(sizeof(lastSecurityId_) == 8, "Expected 64-bit integer");
    log::fatal("Engine::assignSecurityId: Ran out of 64-bit securityIds");
  }
  return lastSecurityId_;
}

#endif  // LIBOFP_ENABLE_OPENSSL

void Engine::run() {
  if (!isRunning_) {
    // Set isRunning_ to true when we are in io.run(). This guards against
    // re-entry and provides a flag to test when shutting down.

    isRunning_ = true;
    asyncIdle();
    io_.run();
    idleTimer_.cancel();
    isRunning_ = false;
  }
}

void Engine::stop(Milliseconds timeout) {
  if (timeout == 0_ms) {
    io_.stop();
  } else {
    stopTimer_.expires_after(timeout);
    stopTimer_.async_wait([this](const asio::error_code &err) {
      if (err != asio::error::operation_aborted) {
        io_.stop();
      }
    });
  }
}

bool Engine::registerDatapath(Connection *channel) {
  DatapathID dpid = channel->datapathId();
  UInt8 auxID = channel->auxiliaryId();

  if (auxID == 0) {
    assert(!IsChannelTransportUDP(channel->transport()));

    // Insert main connection's datapathID into the dpidMap, if not present
    // already.
    auto pair = dpidMap_.insert({dpid, channel});
    if (!pair.second) {
      // DatapathID already exists in map. If the existing channel is
      // different, close it and replace it with the new one.
      auto item = pair.first;
      if (item->second != channel) {
        log_error("Datapath conflict between main connections detected:", dpid,
                  std::make_pair("prev_conn_id", item->second->connectionId()),
                  std::make_pair("conn_id", channel->connectionId()));
        Connection *old = item->second;
        item->second = channel;
        old->shutdown(true);  // force immediate reset

      } else {
        log_warning("Datapath is already registered:", dpid,
                    std::make_pair("conn_id", channel->connectionId()));
      }
    }

  } else {
    assert(auxID != 0);

    // Look up main connection and assign auxiliary connection to it. If we
    // don't find a main connection, close the auxiliary channel.
    auto item = dpidMap_.find(dpid);
    if (item != dpidMap_.end()) {
      Connection *parent = item->second;
      if (parent->flags() & Connection::kPermitsAuxiliary) {
        channel->setMainConnection(parent, auxID);
      } else {
        log_warning(
            "registerDatapath: Auxiliary connection not permitted for datapath",
            dpid, "aux", static_cast<int>(auxID),
            std::make_pair("conn_id", channel->connectionId()));
        return false;
      }

    } else {
      log_warning("registerDatapath: Main connection not found for datapath",
                  dpid, "aux", static_cast<int>(auxID),
                  std::make_pair("conn_id", channel->connectionId()));
      return false;
    }
  }

  return true;
}

void Engine::releaseDatapath(Connection *channel) {
  DatapathID dpid = channel->datapathId();
  UInt8 auxID = channel->auxiliaryId();

  // We only need to release the datapath for main connections.
  if (auxID == 0) {
    // When releasing the datapathID for a main connection, we need to
    // verify that the datapathID _is_ registered to this connection.
    auto item = dpidMap_.find(dpid);
    if (item != dpidMap_.end()) {
      if (item->second == channel)
        dpidMap_.erase(item);
    }
  }
}

UInt64 Engine::registerServer(TCP_Server *server) {
  assert(!serverListLock_);
  serverList_.push_back(server);
  return assignConnectionId();
}

void Engine::releaseServer(TCP_Server *server) {
  assert(!serverListLock_);
  auto iter = std::find(serverList_.begin(), serverList_.end(), server);
  if (iter != serverList_.end()) {
    assert(*iter == server);
    serverList_.erase(iter);
  }
}

UInt64 Engine::registerConnection(Connection *connection) {
  assert(!connListLock_);
  connList_.push_back(connection);
  return assignConnectionId();
}

void Engine::releaseConnection(Connection *connection) {
  assert(!connListLock_);
  auto iter = findConnIter(connection->connectionId());
  if (iter != connList_.end()) {
    assert(*iter == connection);
#if __GNUC__ == 4 && __GNUC_MINOR__ == 8
    // Convert const_iterator to iterator.
    auto it = connList_.begin() + (iter - connList_.begin());
    connList_.erase(it);
#else
    connList_.erase(iter);
#endif
  }
}

static const char *sSigname(int signum) {
  switch (signum) {
    case SIGTERM:
      return "SIGTERM";
    case SIGINT:
      return "SIGINT";
    case SIGHUP:
      return "SIGHUP";
  }
  return "???";
}

void Engine::installSignalHandlers(std::function<void()> callback) {
  log_debug("Install signal handlers");

  std::error_code ignore;
  signals_.cancel(ignore);
  signals_.clear(ignore);
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
  signals_.add(SIGHUP);

  OFP_BEGIN_IGNORE_PADDING

  signals_.async_wait(
      [this, callback](const asio::error_code &error, int signum) {
        if (!error) {
          // Log and ignore SIGHUP.
          if (signum == SIGHUP) {
            log_info("Signal ignored:", sSigname(signum));
            return;
          }

          log_info("Signal received:", sSigname(signum));

          signals_.cancel();
          idleTimer_.cancel();
          (void)this->closeAll();

          if (callback)
            callback();

        } else {
          log_error("Signal error", signum, error);
        }
      });

  OFP_END_IGNORE_PADDING
}

UInt64 Engine::assignConnectionId() {
  if (++lastConnId_ == 0) {
    static_assert(sizeof(lastConnId_) == 8, "Expected 64-bit integer");
    log::fatal("Engine::assignConnectionId: Ran out of 64-bit connectionIds");
  }
  return lastConnId_;
}

Connection *Engine::findDatapath(UInt64 connId, const DatapathID &dpid) const {
  bool dpidEmpty = dpid.empty();

  // Use the connectionId, it it's non-zero. If a datapathID is also provided,
  // it must match also.
  if (connId != 0) {
    Connection *conn = findConnId(connId);
    if (conn != nullptr) {
      if (dpidEmpty || conn->datapathId() == dpid) {
        return conn;
      } else {
        log_warning("findDatapath: conn_id with mismatched datapath", connId);
      }
    }
    return nullptr;
  }

  // If datapath ID is not all zeros, use it to look up the connection.
  if (!dpidEmpty) {
    auto item = dpidMap_.find(dpid);
    if (item != dpidMap_.end()) {
      return item->second;
    }
    return nullptr;
  }

  assert(dpidEmpty && connId == 0);
  log_warning("findDatapath: no conn_id or datapath_id specified");

  return nullptr;
}

Connection *Engine::findConnId(UInt64 connId) const {
  auto iter = findConnIter(connId);
  if (iter != connList_.end()) {
    return *iter;
  }
  return nullptr;
}

std::vector<Connection *>::const_iterator Engine::findConnIter(
    UInt64 connId) const {
  assert(connId != 0);

  // Use binary search to locate connection with connID.
  auto iter = std::lower_bound(
      connList_.begin(), connList_.end(), connId,
      [](Connection *conn, UInt64 cid) { return conn->connectionId() < cid; });

  if (iter != connList_.end()) {
    Connection *conn = *iter;
    if (conn->connectionId() == connId)
      return iter;
  }

  return connList_.end();
}

void Engine::asyncIdle() {
  idleTimer_.expires_after(1000_ms);
  idleTimer_.async_wait([this](const asio::error_code &err) {
    if (!err) {
      TimePoint now = TimeClock::now();
      forEachConnection([&now](Connection *conn) {
        if (conn->flags() & Connection::kConnectionUp) {
          conn->tickle(now);
        }
      });
      asyncIdle();
    }
  });
}

void Engine::setAlertCallback(AlertCallback callback, void *context) {
  alertCallback_ = callback;
  alertContext_ = context;
}

void Engine::alert(Channel *conn, const std::string &alert,
                   const ByteRange &data) {
  if (alertCallback_) {
    alertCallback_(conn, alert, data, alertContext_);
  }
}
