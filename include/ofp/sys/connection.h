//  ===== ---- ofp/sys/connection.h ------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the sys::Connection class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_CONNECTION_H_
#define OFP_SYS_CONNECTION_H_

#include "ofp/channel.h"
#include "ofp/channellistener.h"
#include "ofp/defaulthandshake.h"
#include "ofp/sys/connectiontimer.h"

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
  Connection(Engine *engine, DefaultHandshake *handshake)
      : engine_{engine}, listener_{handshake}, handshake_{handshake},
        mainConn_{this} {}
  virtual ~Connection();

  Driver *driver() const override;

  UInt8 version() const override { return version_; }
  void setVersion(UInt8 version) { version_ = version; }

  DatapathID datapathId() const override { return datapathId_; }
  UInt8 auxiliaryId() const override { return auxiliaryId_; }

  Connection *mainConnection() const { return mainConn_; }

  void setMainConnection(Connection *channel, UInt8 auxID);

  // Connection *nextAuxiliaryConnection() const {
  //	return nextAuxConn_;
  //}

  // void setNextAuxiliaryConnection(Connection *channel) {
  //	nextAuxConn_ = channel;
  //}

  UInt32 nextXid() override { return nextXid_++; }

  ChannelListener *channelListener() const override { return listener_; }
  void setChannelListener(ChannelListener *listener) override {
    listener_ = listener;
  }

  DefaultHandshake *handshake() const { return handshake_; }
  void setHandshake(DefaultHandshake *handshake) { handshake_ = handshake; }

  void postMessage(Connection *source, Message *message);
  void postTimerExpired(ConnectionTimer *timer);
  void postIdle();
  void postDatapathId(const DatapathID &datapath, UInt8 auxiliaryId);

  sys::Engine *engine() const { return engine_; }

  void scheduleTimer(UInt32 timerID, Milliseconds when, bool repeat) override;
  void cancelTimer(UInt32 timerID) override;

  void setStartingXid(UInt32 xid) override { nextXid_ = xid; }

  void openAuxChannel(UInt8 auxID, Transport transport) override;
  Channel *findAuxChannel(UInt8 auxID) const override { return nullptr; }

private:
  using AuxiliaryList = std::vector<Connection *>;

  sys::Engine *engine_;
  ChannelListener *listener_ = nullptr;
  DefaultHandshake *handshake_ = nullptr;
  Connection *mainConn_;
  AuxiliaryList auxList_;
  ConnectionTimerMap timers_;
  DatapathID datapathId_;
  UInt32 nextXid_ = 0;
  UInt8 version_ = 0;
  UInt8 auxiliaryId_ = 0;
  bool dpidWasPosted_ = false;
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_CONNECTION_H_
