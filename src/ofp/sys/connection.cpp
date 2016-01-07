// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/sys/connection.h"
#include "ofp/sys/engine.h"
#include "ofp/message.h"
#include "ofp/echorequest.h"
#include "ofp/echoreply.h"

using namespace ofp;
using namespace ofp::sys;

const Milliseconds kKeepAliveDefaultTimeout = 10000_ms;
const UInt32 kKeepAliveEchoXID = 0xFBD0FF86;
const ByteRange kKeepAliveEchoData{"Bueller?", 8};

Connection::Connection(Engine *engine, DefaultHandshake *handshake)
    : engine_{engine},
      listener_{handshake},  // handshake_{handshake},
      mainConn_{this},
      keepAliveTimeout_{kKeepAliveDefaultTimeout} {
  connId_ = engine_->registerConnection(this);
  updateTimeReadStarted();
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
        if (conn->flags() & kManualDelete) {
          delete conn;
        }
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

Driver *Connection::driver() const {
  return engine_->driver();
}

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

  log::trace_msg("Read", message->source()->connectionId(), message->data(),
                 message->size());

  // Once Hello's have been exchanged, handle incoming echo requests
  // automatically. Change the type to echo reply and send it right back.
  if (message->type() == OFPT_ECHO_REQUEST && version() >= OFP_VERSION_1) {
    message->mutableHeader()->setType(OFPT_ECHO_REPLY);
    write(message->data(), message->size());
    flush();
    return;  // all done!
  }

  // Ignore incoming echo replies that originate from our "keep-alive" echo
  // requests sent when the connection is idle.
  if (message->type() == OFPT_ECHO_REPLY &&
      message->xid() == kKeepAliveEchoXID) {
    const EchoReply *reply = EchoReply::cast(message);
    if (reply && reply->echoData() == kKeepAliveEchoData) {
      return;  // all done!
    }
  }

  ChannelListener *listener = mainConn_->listener_;
  if (listener) {
    message->transmogrify();
    listener->onMessage(message);
  }
}

bool Connection::postDatapath(const DatapathID &datapathId, UInt8 auxiliaryId) {
  if (auxiliaryId == 0 && IsChannelTransportUDP(transport())) {
    log::error("UDP connection not allowed to have auxiliary_id of 0",
               std::make_pair("connid", connectionId()));
    return false;
  }

  bool result = false;

  if (!datapathId.empty()) {
    datapathId_ = datapathId;
    auxiliaryId_ = auxiliaryId;
    result = engine()->registerDatapath(this);
    if (result) {
      log::info("Assign datapath", datapathId, "aux",
                static_cast<int>(auxiliaryId),
                std::make_pair("conn_id", connectionId()));
    }
  }

  return result;
}

void Connection::poll() {
  auto age = std::chrono::steady_clock::now() - timeReadStarted_;
  if (age < keepAliveTimeout_)
    return;

  if (version() < OFP_VERSION_1 || age >= 2 * keepAliveTimeout_) {
    // Keep alive timeout has expired.
    engine()->alert(this, "No response to echo request on idle channel", {});
    shutdown();
  } else if (!(flags() & kChannelIdle)) {
    setFlags(flags() | kChannelIdle);
    EchoRequestBuilder echoReq{kKeepAliveEchoXID};
    echoReq.setEchoData(kKeepAliveEchoData.data(), kKeepAliveEchoData.size());
    echoReq.send(this);
  }
}

void Connection::channelUp() {
  if (!(flags() & kChannelUp)) {
    log::debug("channelUp", std::make_pair("connid", connectionId()));
    setFlags(flags() | kChannelUp);
    auto delegate = channelListener();
    if (delegate) {
      delegate->onChannelUp(this);
    }
  }
}

void Connection::channelDown() {
  if (flags() & kChannelUp) {
    log::debug("channelDown", std::make_pair("connid", connectionId()));
    setFlags(flags() & ~kChannelUp);
    auto delegate = channelListener();
    if (delegate) {
      delegate->onChannelDown(this);
    }
  }
}

void Connection::updateTimeReadStarted() {
  timeReadStarted_ = std::chrono::steady_clock::now();
  setFlags(flags() & ~kChannelIdle);
}
