//  ===== ---- ofp/sys/tcp_connection.cpp ------------------*- C++ -*- =====  //
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
/// \brief Implements sys::TCP_Connection class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/sys/tcp_connection.h"
#include "ofp/sys/tcp_server.h"
#include "ofp/sys/engine.h"
#include "ofp/log.h"
#include "ofp/defaulthandshake.h"

using namespace ofp::sys;

TCP_Connection::TCP_Connection(Engine *engine, Driver::Role role,
                               ProtocolVersions versions,
                               ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, role, versions, factory}},
      message_{this}, socket_{engine->io()}, idleTimer_{engine->io()} {}

TCP_Connection::TCP_Connection(Engine *engine, tcp::socket socket,
                               Driver::Role role, ProtocolVersions versions,
                               ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, role, versions, factory}},
      message_{this}, socket_{std::move(socket)}, idleTimer_{engine->io()} {}

/// \brief Construct connection object for reconnect attempt.
TCP_Connection::TCP_Connection(Engine *engine, DefaultHandshake *handshake)
    : Connection{engine, handshake}, message_{this}, socket_{engine->io()},
      idleTimer_{engine->io()} {
  handshake->setConnection(this);
}

TCP_Connection::~TCP_Connection() {}

ofp::IPv6Endpoint TCP_Connection::remoteEndpoint() const {
  if (isOutgoing()) {
    return convertEndpoint<tcp>(endpoint_);
  } else {
    return convertEndpoint<tcp>(socket_.lowest_layer().remote_endpoint());
  }
}

void TCP_Connection::write(const void *data, size_t length) {
  socket_.buf_write(data, length);
}

void TCP_Connection::flush() { socket_.buf_flush(); }

void TCP_Connection::shutdown() {
  log::debug(__PRETTY_FUNCTION__);

  // FIXME -- this should stop reading data and close connection when
  // all outgoing data existing at this point is sent.
  socket_.lowest_layer().close();
}

ofp::Deferred<ofp::Exception>
TCP_Connection::asyncConnect(const tcp::endpoint &endpt, Milliseconds delay) {
  assert(deferredExc_ == nullptr);

  endpoint_ = endpt;
  deferredExc_ = Deferred<Exception>::makeResult();

  if (delay > 0_ms) {
    asyncDelayConnect(delay);

  } else {
    asyncConnect();
  }

  return deferredExc_;
}

void TCP_Connection::asyncAccept() {
  // Do nothing if socket is not open.
  if (!socket_.lowest_layer().is_open())
    return;

  // We always send and receive complete messages; disable Nagle algorithm.
  socket_.lowest_layer().set_option(tcp::no_delay(true));

  channelUp();
}

void TCP_Connection::channelUp() {
  assert(channelListener());

  channelListener()->onChannelUp(this);
  asyncReadHeader();

  updateLatestActivity();
  asyncIdleCheck();
}

void TCP_Connection::channelException(const Exception &exc) {
  assert(channelListener());

  channelListener()->onException(&exc);
}

void TCP_Connection::channelDown() {
  assert(channelListener());

  channelListener()->onChannelDown(this);

  if (wantsReconnect()) {
    reconnect();
  }
}

void TCP_Connection::asyncReadHeader() {
  // Do nothing if socket is not open.
  if (!socket_.lowest_layer().is_open())
    return;

  auto self(shared_from_this());

  asio::async_read(socket_, asio::buffer(message_.mutableData(sizeof(Header)),
                                         sizeof(Header)),
                   [this, self](const asio::error_code &err, size_t length) {

    if (!err) {
      assert(length == sizeof(Header));
      const Header *hdr = message_.header();

      if (hdr->validateInput(version())) {
        // The header has passed our rudimentary validation checks.
        UInt16 msgLength = hdr->length();

        if (msgLength == sizeof(Header)) {
          postMessage(this, &message_);
          asyncReadHeader();

        } else {
          asyncReadMessage(msgLength);
        }
      }

    } else {

      if (err != asio::error::eof) {
        auto exc = makeException(err);
        log::debug("asyncReadHeader err ", exc);
        channelException(makeException(err));
      }

      channelDown();
    }

    updateLatestActivity();
  });
}

void TCP_Connection::asyncReadMessage(size_t msgLength) {
  assert(msgLength > sizeof(Header));

  auto self(shared_from_this());

  asio::async_read(
      socket_, asio::buffer(message_.mutableData(msgLength) + sizeof(Header),
                            msgLength - sizeof(Header)),
      [this, self](const asio::error_code &err, size_t bytes_transferred) {

        if (!err) {
          assert(bytes_transferred == message_.size() - sizeof(Header));

          postMessage(this, &message_);
          asyncReadHeader();

        } else {
          if (err != asio::error::eof) {
            auto exc = makeException(err);
            log::info("asyncReadMessage err ", exc);
            channelException(makeException(err));
          }
          channelDown();
        }

        updateLatestActivity();
      });
}

void TCP_Connection::asyncConnect() {
  auto self(shared_from_this());

  socket_.lowest_layer().async_connect(
      endpoint_, [this, self](const asio::error_code &err) {
        // `async_connect` may not report an error when the connection attempt
        // fails. We need to double-check that we are connected.

        if (!err) {
          socket_.lowest_layer().set_option(tcp::no_delay(true));
          channelUp();

        } else if (wantsReconnect()) {
          reconnect();
        }

        deferredExc_->done(makeException(err));
        deferredExc_ = nullptr;
      });
}

void TCP_Connection::asyncDelayConnect(Milliseconds delay) {
  auto self(shared_from_this());

  asio::error_code error;
  idleTimer_.expires_from_now(delay, error);
  if (error)
    return;

  idleTimer_.async_wait([this, self](const asio::error_code &err) {
    if (err != asio::error::operation_aborted) {
      asyncConnect();
    } else {
      assert(deferredExc_ != nullptr);
      deferredExc_->done(makeException(err));
      deferredExc_ = nullptr;
    }
  });
}

void TCP_Connection::asyncIdleCheck() {
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  auto interval =
      std::chrono::duration_cast<Milliseconds>(now - latestActivity_);

  Milliseconds delay;
  if (interval >= 5000_ms) {
    postIdle();
    delay = 5000_ms;
  } else {
    delay = 5000_ms - interval;
  }

  asio::error_code error;
  idleTimer_.expires_from_now(delay, error);
  if (error)
    return;

  idleTimer_.async_wait([this](const asio::error_code &err) {
    if (err != asio::error::operation_aborted) {
      asyncIdleCheck();
    }
  });
}

void TCP_Connection::reconnect() {
  DefaultHandshake *hs = handshake();
  assert(hs);

  hs->setStartingVersion(version());
  hs->setStartingXid(nextXid());

  log::debug("reconnecting...", remoteEndpoint());

  engine()->reconnect(hs, remoteEndpoint(), 750_ms);

  setHandshake(nullptr);
  if (channelListener() == hs) {
    setChannelListener(nullptr);
  }

  assert(channelListener() != hs);
}

void TCP_Connection::updateLatestActivity() {
  latestActivity_ = std::chrono::steady_clock::now();
}
