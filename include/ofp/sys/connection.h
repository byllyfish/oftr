// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_SYS_CONNECTION_H_
#define OFP_SYS_CONNECTION_H_

#include "ofp/channel.h"
#include "ofp/channellistener.h"
#include "ofp/defaulthandshake.h"

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

  void postMessage(Message *message);
  void postIdle();
  bool postDatapath(const DatapathID &datapath, UInt8 auxiliaryId);

  sys::Engine *engine() const { return engine_; }

  void setStartingXid(UInt32 xid) override { nextXid_ = xid; }

  /// Does this class require a delete() to finish shutdown? (See
  /// UDP_Connection.) FIXME(bfish) - replace this with a flags_ field.
  virtual bool shutdownRequiresManualDelete() const { return false; }

 private:
  using AuxiliaryList = std::vector<Connection *>;

  sys::Engine *engine_;
  ChannelListener *listener_ = nullptr;
  Connection *mainConn_;
  AuxiliaryList auxList_;
  DatapathID datapathId_;
  UInt64 connId_ = 0;
  UInt32 nextXid_ = 0;
  UInt8 version_ = 0;
  UInt8 auxiliaryId_ = 0;
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_CONNECTION_H_
