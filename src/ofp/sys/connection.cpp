//  ===== ---- ofp/sys/connection.cpp ----------------------*- C++ -*- =====  //
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
/// \brief Implements sys::Connection class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/sys/connection.h"
#include "ofp/sys/engine.h"
#include "ofp/message.h"

using namespace ofp;
using namespace ofp::sys;

Connection::Connection(Engine *engine, DefaultHandshake *handshake)
    : engine_{engine}, listener_{handshake}, //handshake_{handshake},
      mainConn_{this} {

  connId_ = engine_->registerConnection(this);
}

Connection::~Connection() {
  ChannelListener::dispose(listener_);

  if (!datapathId_.empty()) {
    engine()->releaseDatapath(this);

    if (mainConn_ == this) {
      // Main connection is going down. Terminate all of our auxiliary
      // connections.
      for (auto conn : auxList_) {
        conn->mainConn_ = conn;
        conn->shutdown();
      }

    } else {
      assert(mainConn_ != this);

      // Remove ourselves from the auxiliary connection list of our main
      // connection.
      assert(mainConn_ != nullptr);
      AuxiliaryList &auxList = mainConn_->auxList_;
      auto iter = std::find(auxList.begin(), auxList.end(), this);
      if (iter != auxList.end()) {
        auxList.erase(iter);
      } else {
        log::debug("~Connection: Missing pointer to auxiliary connection.");
      }
    }
  }

  engine()->releaseConnection(this);
}

Driver *Connection::driver() const { return engine_->driver(); }

void Connection::setMainConnection(Connection *channel, UInt8 auxID) {
  assert(channel != nullptr);
  assert(mainConn_ == this);
  assert(channel != this);
  assert(auxID != 0);

  log::debug("setMainConnection");
  mainConn_ = channel;
  datapathId_ = mainConn_->datapathId();
  auxiliaryId_ = auxID;

  AuxiliaryList &auxList = mainConn_->auxList_;

  // Check if there is already an auxiliary connection with the same ID.
  // If so, close it so we can replace it with this one.

  auto iter = std::find_if(
      auxList.begin(), auxList.end(),
      [auxID](Connection *conn) { return conn->auxiliaryId() == auxID; });
  if (iter != auxList.end()) {
    log::info("setMainConnection: Auxiliary connection found with same ID.");
    (*iter)->shutdown();
  }

  auxList.push_back(this);
}

void Connection::postMessage(Message *message) {
  assert(message->source());

  log::trace("Read", message->source()->connectionId(), message->data(), message->size());

  // Handle echo reply automatically. We just set the type to reply and write
  // it back.
  if (message->type() == OFPT_ECHO_REQUEST) {
    message->mutableHeader()->setType(OFPT_ECHO_REPLY);
    write(message->data(), message->size());
    flush();
    return; // all done!
  }

  ChannelListener *listener = mainConn_->listener_;
  if (listener) {
    message->transmogrify();
    listener->onMessage(message);
  }
}

void Connection::postIdle() { log::debug("postIdle() =========="); }

void Connection::postDatapath(const DatapathID &datapathId,
                                UInt8 auxiliaryId) {
  if (!datapathId.empty()) {
    datapathId_ = datapathId;
    auxiliaryId_ = auxiliaryId;
    engine()->registerDatapath(this);

    log::info("Assign datapath_id", datapathId, "auxiliary_id", int(auxiliaryId), std::make_pair("conn_id", connectionId()));
  }
}
