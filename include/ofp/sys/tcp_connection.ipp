//  ===== ---- ofp/sys/tcp_connection.ipp ------------------*- C++ -*- =====  //
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

namespace ofp {
namespace sys {


template <class SocketType>
TCP_Connection<SocketType>::TCP_Connection(Engine *engine, Driver::Role role,
                               ProtocolVersions versions,
                               ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, role, versions, factory}},
      message_{this}, socket_{engine->io(), engine->context()}, idleTimer_{engine->io()} {}

template <class SocketType>
TCP_Connection<SocketType>::TCP_Connection(Engine *engine, tcp::socket socket,
                               Driver::Role role, ProtocolVersions versions,
                               ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, role, versions, factory}},
      message_{this}, socket_{std::move(socket), engine->context()}, idleTimer_{engine->io()} {}

/// \brief Construct connection object for reconnect attempt.
template <class SocketType>
TCP_Connection<SocketType>::TCP_Connection(Engine *engine, DefaultHandshake *handshake)
    : Connection{engine, handshake}, message_{this}, socket_{engine->io(), engine->context()},
      idleTimer_{engine->io()} {
  handshake->setConnection(this);
}

template <class SocketType>
ofp::IPv6Endpoint TCP_Connection<SocketType>::remoteEndpoint() const {
  if (isOutgoing()) {
    return convertEndpoint<tcp>(endpoint_);
  } else {
    return convertEndpoint<tcp>(socket_.lowest_layer().remote_endpoint());
  }
}

template <class SocketType>
void TCP_Connection<SocketType>::flush() {
  auto self(this->shared_from_this());
  socket_.buf_flush([this, self](const std::error_code &error){
    if (error) {
      socket_.lowest_layer().close();
    }
  });
}

template <class SocketType>
void TCP_Connection<SocketType>::shutdown() {
  auto self(this->shared_from_this());
  socket_.async_shutdown([this, self](const std::error_code &error){
    socket_.shutdownLowestLayer();
  });
}

template <class SocketType>
ofp::Deferred<std::error_code>
TCP_Connection<SocketType>::asyncConnect(const tcp::endpoint &endpt, Milliseconds delay) {
  assert(deferredExc_ == nullptr);

  endpoint_ = endpt;
  deferredExc_ = Deferred<std::error_code>::makeResult();

  if (delay > 0_ms) {
    asyncDelayConnect(delay);

  } else {
    asyncConnect();
  }

  return deferredExc_;
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncAccept() {
  // Do nothing if socket is not open.
  if (!socket_.is_open())
    return;

  // We always send and receive complete messages; disable Nagle algorithm.
  socket_.lowest_layer().set_option(tcp::no_delay(true));

  asyncHandshake();
}

template <class SocketType>
void TCP_Connection<SocketType>::channelUp() {
  assert(channelListener());

  channelListener()->onChannelUp(this);
  asyncReadHeader();

  updateLatestActivity();
  asyncIdleCheck();
}

template <class SocketType>
void TCP_Connection<SocketType>::channelDown() {
  assert(channelListener());

  channelListener()->onChannelDown(this);

  if (wantsReconnect()) {
    reconnect();
  }
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncReadHeader() {
  // Do nothing if socket is not open.
  if (!socket_.is_open()) {
    log::debug("asyncReadHeader called with socket closed.");
    return;
  }

  auto self(this->shared_from_this());

  asio::async_read(socket_, asio::buffer(message_.mutableData(sizeof(Header)),
                                         sizeof(Header)),
                   [this, self](const asio::error_code &err, size_t length) {
    log::Lifetime lifetime{"asyncReadHeader callback."};

    if (!err) {
      assert(length == sizeof(Header));
      const Header *hdr = message_.header();

      if (hdr->validateInput(version())) {
        // The header has passed our rudimentary validation checks.
        UInt16 msgLength = hdr->length();

        if (msgLength == sizeof(Header)) {
          postMessage(this, &message_);
          if (socket_.is_open()) {
            asyncReadHeader();
          } else {
            // Rare: postMessage() closed the socket forcefully.
            channelDown();
          }

        } else {
          asyncReadMessage(msgLength);
        }
      } else {
        // The header failed our rudimentary validation checks.
        log::debug("asyncReadHeader header validation failed");
        channelDown();
      }

    } else {

      if (err != asio::error::eof) {
        log::debug("asyncReadHeader error ", err);
      }

      channelDown();
    }

    updateLatestActivity();
  });
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncReadMessage(size_t msgLength) {
  assert(msgLength > sizeof(Header));
  assert(socket_.is_open());

  auto self(this->shared_from_this());

  asio::async_read(
      socket_, asio::buffer(message_.mutableData(msgLength) + sizeof(Header),
                            msgLength - sizeof(Header)),
      [this, self](const asio::error_code &err, size_t bytes_transferred) {

        if (!err) {
          assert(bytes_transferred == message_.size() - sizeof(Header));

          postMessage(this, &message_);
          if (socket_.is_open()) {
            asyncReadHeader();
          } else {
            // Rare: postMessage() closed the socket forcefully.
            channelDown();
          }

        } else {
          if (err != asio::error::eof) {
            log::info("asyncReadMessage error ", err);
          }
          channelDown();
        }

        updateLatestActivity();
      });
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncHandshake() {
  // Start async handshake.
  auto mode = isOutgoing() ? asio::ssl::stream_base::client : asio::ssl::stream_base::server;

  auto self(this->shared_from_this());
  socket_.async_handshake(mode, [this, self](const asio::error_code &err) {
    if (!err) {
      channelUp();
    } else {
      log::debug("async_handshake failed", err.message());
    }
  });
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncConnect() {
  auto self(this->shared_from_this());

  socket_.lowest_layer().async_connect(
      endpoint_, [this, self](const asio::error_code &err) {
        // `async_connect` may not report an error when the connection attempt
        // fails. We need to double-check that we are connected.

        if (!err) {
          socket_.lowest_layer().set_option(tcp::no_delay(true));
          asyncHandshake();

        } else if (wantsReconnect()) {
          reconnect();
        }

        deferredExc_->done(err);
        deferredExc_ = nullptr;
      });
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncDelayConnect(Milliseconds delay) {
  auto self(this->shared_from_this());

  asio::error_code error;
  idleTimer_.expires_from_now(delay, error);
  if (error)
    return;

  idleTimer_.async_wait([this, self](const asio::error_code &err) {
    if (err != asio::error::operation_aborted) {
      asyncConnect();
    } else {
      assert(deferredExc_ != nullptr);
      deferredExc_->done(err);
      deferredExc_ = nullptr;
    }
  });
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncIdleCheck() {
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

template <class SocketType>
void TCP_Connection<SocketType>::reconnect() {
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

}  // namespace sys
}  // namespace ofp
