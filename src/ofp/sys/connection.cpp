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
    : engine_{engine}, listener_{handshake}, handshake_{handshake},
      mainConn_{this} {

  connId_ = engine_->registerConnection(this);
}

Connection::~Connection() {
  if (listener_ != handshake_) {
    ChannelListener::dispose(listener_);
  }
  ChannelListener::dispose(handshake_);

  if (datapathRegistered_ && engine()->isRunning()) {   // FIXME: necessary to check if engine running?
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

void Connection::postMessage(Connection *source, Message *message) {
  log::trace("read", message->data(), message->size());

  // Handle echo reply automatically. We just set the type to reply and write
  // it back.
  if (message->type() == OFPT_ECHO_REQUEST) {
    message->mutableHeader()->setType(OFPT_ECHO_REPLY);
    write(message->data(), message->size());
    flush();
    return; // all done!
  }

  if (listener_) {
    message->transmogrify();
    listener_->onMessage(message);
  }
}

void Connection::postIdle() { log::debug("postIdle() =========="); }

void Connection::postDatapathId(const DatapathID &datapathId,
                                UInt8 auxiliaryId) {
  assert(!datapathRegistered_);

  datapathId_ = datapathId;
  auxiliaryId_ = auxiliaryId;
  datapathRegistered_ = true; // FIXME - replace with check for all-0 datapath?

  engine()->registerDatapath(this);
}

void Connection::openAuxChannel(UInt8 auxID, Transport transport) {
  engine()->openAuxChannel(auxID, transport, this);
}
