// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/connection.h"
#include "ofp/echoreply.h"
#include "ofp/echorequest.h"
#include "ofp/message.h"
#include "ofp/sys/engine.h"

using namespace ofp;
using namespace ofp::sys;

const Milliseconds kKeepAliveDefaultTimeout = 10000_ms;

Connection::Connection(Engine *engine, DefaultHandshake *handshake)
    : engine_{engine},
      listener_{handshake},
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
      auto &auxList = mainConn_->auxList_;
      auto iter = std::find(auxList.begin(), auxList.end(), this);
      if (iter != auxList.end()) {
        auxList.erase(iter);
      } else {
        log_debug("~Connection: Missing pointer to auxiliary connection.");
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

  log_debug("setMainConnection");
  mainConn_ = channel;
  datapathId_ = mainConn_->datapathId();
  auxiliaryId_ = auxID;

  auto &auxList = mainConn_->auxList_;

  // Check if there is already an auxiliary connection with the same ID.
  // If so, close it so we can replace it with this one.

  auto iter = std::find_if(
      auxList.begin(), auxList.end(),
      [auxID](Connection *conn) { return conn->auxiliaryId() == auxID; });
  if (iter != auxList.end()) {
    log_info("setMainConnection: Auxiliary connection found with same ID.");
    (*iter)->shutdown();
  }

  auxList.push_back(this);
}

void Connection::postMessage(Message *message) {
  assert(message->source());

  // Assign message timestamp here.
  message->setTime(Timestamp::now());

  log::trace_msg("Read", message->source()->connectionId(), message->data(),
                 message->size());

  if (version() >= OFP_VERSION_1 && echoMessageHandled(message)) {
    return;
  }

  ChannelListener *listener = mainConn_->listener_;
  if (listener) {
    message->normalize();
    listener->onMessage(message);
  }
}

bool Connection::postDatapath(const DatapathID &datapathId, UInt8 auxiliaryId) {
  if (auxiliaryId == 0 && IsChannelTransportUDP(transport())) {
    log_error("UDP connection not allowed to have auxiliary_id of 0",
              std::make_pair("connid", connectionId()));
    return false;
  }

  if (datapathId.empty()) {
    log_error("Datapath is not allowed to be all zeros", std::make_pair("connid", connectionId()));
    return false;
  }

  datapathId_ = datapathId;
  auxiliaryId_ = auxiliaryId;
  
  bool result = engine()->registerDatapath(this);
  if (result) {
    log_info("Assign datapath", datapathId, "aux",
             static_cast<int>(auxiliaryId),
             std::make_pair("conn_id", connectionId()));
  }
  return result;
}

void Connection::tickle(TimePoint now) {
  assert((flags() & kConnectionUp) != 0);

  // Give channel listener first dibs on the tickle.
  if (listener_ && listener_->onTickle(this, now)) {
    return;
  }

  auto age = now - timeReadStarted_;
  if (age < keepAliveTimeout_)
    return;

  if (version() < OFP_VERSION_1 || age >= 2 * keepAliveTimeout_) {
    // Keep alive timeout has expired.
    engine()->alert(this, "No response to echo request on idle channel", {});
    shutdown();
  } else if (!(flags() & kChannelIdle)) {
    setFlags(flags() | kChannelIdle);
    EchoRequestBuilder echoReq{0};
    echoReq.setKeepAlive();
    echoReq.send(this);
  }
}

void Connection::channelUp() {
  if (!(flags() & kChannelUp)) {
    log_debug("channelUp", std::make_pair("connid", connectionId()));
    setFlags(flags() | kChannelUp);
    auto delegate = channelListener();
    if (delegate) {
      delegate->onChannelUp(this);
    }
  }
}

void Connection::channelDown() {
  if (flags() & kChannelUp) {
    log_debug("channelDown", std::make_pair("connid", connectionId()));
    setFlags(flags() & ~kChannelUp);
    auto delegate = channelListener();
    if (delegate) {
      delegate->onChannelDown(this);
    }
  }
}

void Connection::updateTimeReadStarted() {
  timeReadStarted_ = TimeClock::now();
  setFlags(flags() & ~kChannelIdle);
}

void Connection::setFlags(UInt64 securityId, ChannelOptions options) {
  UInt16 newFlags = flags();

  if (securityId != 0) {
    newFlags |= kRequiresHandshake;
  }

  if ((options & ChannelOptions::AUXILIARY) != 0) {
    newFlags |= kPermitsAuxiliary;
  }

  if ((options & ChannelOptions::NO_VERSION_CHECK) != 0) {
    newFlags |= kPermitsOtherVersions;
  }

  if ((options & ChannelOptions::FEATURES_REQ) != 0) {
    newFlags |= kDefaultController;
  }

  setFlags(newFlags);
}

bool Connection::echoMessageHandled(Message *message) {
  const UInt8 type = message->type();

  if (type == OFPT_ECHO_REQUEST) {
    const EchoRequest *request = EchoRequest::cast(message);
    if (request && request->isPassThru()) {
      // Do not handle pass-thru echo requests.
      return false;
    }
    // Change the type to echo reply and send it right back.
    message->mutableHeader()->setType(OFPT_ECHO_REPLY);
    write(message->data(), message->size());
    flush();
    return true;
  }

  if (type == OFPT_ECHO_REPLY) {
    const EchoReply *reply = EchoReply::cast(message);
    if (reply && reply->isKeepAlive()) {
      // Handle keep-alive replies by doing nothing.
      return true;
    }
  }

  return false;
}
