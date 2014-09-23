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
TCP_Connection<SocketType>::TCP_Connection(Engine *engine, ChannelMode mode,
                                           ProtocolVersions versions,
                                           ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, mode, versions, factory}},
      message_{this},
      socket_{engine->io(), engine->context()} {}

template <class SocketType>
TCP_Connection<SocketType>::TCP_Connection(Engine *engine, tcp::socket socket,
                                           ChannelMode mode,
                                           ProtocolVersions versions,
                                           ChannelListener::Factory factory)
    : Connection{engine, new DefaultHandshake{this, mode, versions, factory}},
      message_{this},
      socket_{std::move(socket), engine->context()} {}

template <class SocketType>
TCP_Connection<SocketType>::~TCP_Connection() {
  channelDown();
  log::info("Close TCP connection", std::make_pair("connid", connectionId()));
}

template <class SocketType>
ofp::IPv6Endpoint TCP_Connection<SocketType>::remoteEndpoint() const {
  asio::error_code err;
  return convertEndpoint<tcp>(socket_.lowest_layer().remote_endpoint(err));
}

template <class SocketType>
ofp::IPv6Endpoint TCP_Connection<SocketType>::localEndpoint() const {
  asio::error_code err;
  return convertEndpoint<tcp>(socket_.lowest_layer().local_endpoint(err));
}

template <class SocketType>
void TCP_Connection<SocketType>::flush() {
  auto self(this->shared_from_this());
  socket_.buf_flush(connectionId(), [this, self](const std::error_code &error) {
    if (error) {
      socket_.lowest_layer().close();
    }
  });
}

template <class SocketType>
void TCP_Connection<SocketType>::shutdown() {
  auto self(this->shared_from_this());
  socket_.async_shutdown([this, self](const std::error_code &error) {
    socket_.shutdownLowestLayer();
  });
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncConnect(const IPv6Endpoint &remoteEndpt, std::function<void(Channel*,std::error_code)> resultHandler) {
  auto self(this->shared_from_this());
  tcp::endpoint endpt = convertEndpoint<tcp>(remoteEndpt);

  log::info("Initiate TCP connection to", remoteEndpt, std::make_pair("connid", connectionId()));

  socket_.lowest_layer().async_connect(
      endpt, [this, self, resultHandler](const asio::error_code &err) {
        // `async_connect` may not report an error when the connection attempt
        // fails. We need to double-check that we are connected.

        if (!err) {
          socket_.lowest_layer().set_option(tcp::no_delay(true));
          log::info("Establish TCP connection", localEndpoint(), "-->", remoteEndpoint(), std::make_pair("connid", connectionId()));

          asyncHandshake(true);
        }

        resultHandler(this, err);
      });
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncAccept() {
  // Do nothing if socket is not open.
  if (!socket_.is_open()) return;

  // We always send and receive complete messages; disable Nagle algorithm.
  socket_.lowest_layer().set_option(tcp::no_delay(true));

  log::info("Accept TCP connection", localEndpoint(), "<--", remoteEndpoint(), std::make_pair("connid", connectionId()));

  asyncHandshake(false);
}

template <class SocketType>
void TCP_Connection<SocketType>::channelUp() {
  assert(channelListener());

  channelListener()->onChannelUp(this);
  isChannelUp_ = true;

  asyncReadHeader();
  updateLatestActivity();
}

template <class SocketType>
void TCP_Connection<SocketType>::channelDown() {
  if (isChannelUp_) {
    isChannelUp_ = false;
    if (channelListener()) {
      channelListener()->onChannelDown(this);
    }
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

  asio::async_read(
      socket_,
      asio::buffer(message_.mutableData(sizeof(Header)), sizeof(Header)),
      make_custom_alloc_handler(
          allocator_, [this, self](const asio::error_code &err, size_t length) {
            log::debug("asyncReadHeader callback ", err);
            if (!err) {
              assert(length == sizeof(Header));
              const Header *hdr = message_.header();

              if (hdr->validateInput(version())) {
                // The header has passed our rudimentary validation checks.
                UInt16 msgLength = hdr->length();

                if (msgLength == sizeof(Header)) {
                  postMessage(&message_);
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
          }));
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncReadMessage(size_t msgLength) {
  assert(msgLength > sizeof(Header));
  assert(socket_.is_open());

  auto self(this->shared_from_this());

  asio::async_read(
      socket_, asio::buffer(message_.mutableData(msgLength) + sizeof(Header),
                            msgLength - sizeof(Header)),
      make_custom_alloc_handler(
          allocator_,
          [this, self](const asio::error_code &err, size_t bytes_transferred) {
            log::debug("asyncReadMessage callback ", err);
            if (!err) {
              assert(bytes_transferred == message_.size() - sizeof(Header));

              postMessage(&message_);
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
          }));
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncHandshake(bool isClient) {
  // Start async handshake.
  auto mode = isClient ? asio::ssl::stream_base::client
                           : asio::ssl::stream_base::server;

  auto self(this->shared_from_this());
  socket_.async_handshake(mode, [this, self](const asio::error_code &err) {
    if (!err) {
      channelUp();
    } else {
      log::debug("async_handshake failed", err.message());
    }
  });
}

}  // namespace sys
}  // namespace ofp
