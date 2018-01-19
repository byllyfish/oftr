// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_CONNECTION_H_
#define OFP_SYS_CONNECTION_H_

#include "ofp/channel.h"
#include "ofp/channellistener.h"
#include "ofp/sys/defaulthandshake.h"

namespace ofp {

class Message;

namespace sys {
class Engine;

OFP_BEGIN_IGNORE_PADDING

/// Connection is an interface for a channel that can receive messages
/// posted from other Connections. This interface also supports binding
/// auxillary connections to their main connection, and a main connection to a
/// a list of auxiliary connections. It also supports a connection timer.
class Connection : public Channel {
 public:
  Connection(Engine *engine, DefaultHandshake *handshake);
  virtual ~Connection();

  Driver *driver() const override;

  UInt8 version() const override { return version_; }
  void setVersion(UInt8 version) { version_ = version; }

  UInt64 connectionId() const override { return connId_; }
  DatapathID datapathId() const override { return datapathId_; }
  UInt8 auxiliaryId() const override { return auxiliaryId_; }

  Connection *mainConnection() const { return mainConn_; }
  void setMainConnection(Connection *channel, UInt8 auxID);

  UInt32 nextXid() override { return nextXid_++; }

  ChannelListener *channelListener() const override { return listener_; }
  void setChannelListener(ChannelListener *listener) override {
    listener_ = listener;
  }

  Milliseconds keepAliveTimeout() const override { return keepAliveTimeout_; }
  void setKeepAliveTimeout(const Milliseconds &timeout) override {
    keepAliveTimeout_ = timeout;
  }

  void postMessage(Message *message);
  void postIdle();
  bool postDatapath(const DatapathID &datapathId, UInt8 auxiliaryId);

  sys::Engine *engine() const { return engine_; }

  void setStartingXid(UInt32 xid) override { nextXid_ = xid; }

  // UDP subclass implementation needs this to receive datagrams...
  virtual void datagramReceived(const void *data, size_t length) {}

  UInt16 flags() { return flags_; }
  void setFlags(UInt16 flags) { flags_ = flags; }

  enum {
    /// Indicates connection has called shutdown.
    kShutdownCalled = 0x0001,

    /// Indicates creator is responsible for deletion upon shutdown.
    kManualDelete = 0x0002,

    /// Indicates handshake is required to establish connection (e.g. TLS).
    kRequiresHandshake = 0x0004,

    /// Indicates that result of handshake is available (error possible).
    kHandshakeDone = 0x0008,

    /// Indicates that channel up has been sent to channel delegate.
    kChannelUp = 0x0010,

    /// Indicates connection shutdown completed (error possible).
    kShutdownDone = 0x0020,

    /// Indicates connection is idle.
    kChannelIdle = 0x0040,

    /// Indicates permission for auxiliary connections.
    kPermitsAuxiliary = 0x0080,

    /// Indicates permission for other versions after negotiating with HELLO.
    kPermitsOtherVersions = 0x0100,

    /// Indicates channel is associated with a controller.
    kDefaultController = 0x0200,

    /// Indicates underlying connection is connected and handshake has started.
    kConnectionUp = 0x0400
  };

  void tickle(TimePoint now) override;

 protected:
  /// Invoked by subclasses to inform channel delegate that channel is up.
  void channelUp();

  /// Invoked by subclasses to inform channel delegate that channel is down.
  void channelDown();

  /// Invoked by subclasses when an async read is initiated.
  void updateTimeReadStarted();

  /// Convenience function for initializer.
  void setFlags(UInt64 securityId, ChannelOptions options);

 private:
  sys::Engine *engine_;
  ChannelListener *listener_ = nullptr;
  Connection *mainConn_;
  std::vector<Connection *> auxList_;
  DatapathID datapathId_;
  UInt64 connId_ = 0;
  UInt32 nextXid_ = 0;
  UInt16 flags_ = 0;
  UInt8 version_ = 0;
  UInt8 auxiliaryId_ = 0;
  TimePoint timeReadStarted_;
  Milliseconds keepAliveTimeout_;

  bool echoMessageHandled(Message *message);
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_CONNECTION_H_
