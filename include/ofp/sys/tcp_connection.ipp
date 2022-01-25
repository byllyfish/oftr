// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/log.h"
#include "ofp/sys/defaulthandshake.h"
#include "ofp/sys/engine.h"
#include "ofp/sys/securitycheck.h"
#include "ofp/sys/tcp_connection.h"
#include "ofp/sys/tcp_server.h"

namespace ofp {
namespace sys {

namespace detail {

// Every socket type except PlaintextSocket needs an SSL context. Make sure
// one is provided or abort. The SSL context for a PlaintextSocket can be null.

template <class SocketType>
inline asio::ssl::context *tcpContext(Engine *engine, UInt64 securityId) {
#if LIBOFP_ENABLE_OPENSSL
  assert(securityId != 0);
  Identity *identity = engine->findIdentity(securityId);
  log::fatal_if_null(identity, "tcpContext");
  return identity->tlsContext();
#else
  return PlaintextContext();
#endif
}

template <>
inline asio::ssl::context *tcpContext<PlaintextSocket>(Engine *engine,
                                                       UInt64 securityId) {
  assert(securityId == 0);
  // Return a dummy pointer that points to something that exists. I originally
  // returnd `nullptr` here but the undefined sanitizer complained. The
  // PlaintextSocket type does not use or access the asio::ssl::context passed
  // in; the ssl::context here is a placeholder.
  return PlaintextContext();
}

}  // namespace detail

template <class SocketType>
TCP_Connection<SocketType>::TCP_Connection(Engine *engine,
                                           ChannelOptions options,
                                           UInt64 securityId,
                                           ProtocolVersions versions,
                                           ChannelListener::Factory factory)
    : Connection{engine,
                 new DefaultHandshake{this, options, versions, factory}},
      message_{this},
      socket_{engine->io(),
              detail::tcpContext<SocketType>(engine, securityId)} {
  setFlags(securityId, options);
}

template <class SocketType>
TCP_Connection<SocketType>::TCP_Connection(Engine *engine, tcp::socket socket,
                                           ChannelOptions options,
                                           UInt64 securityId,
                                           ProtocolVersions versions,
                                           ChannelListener::Factory factory)
    : Connection{engine,
                 new DefaultHandshake{this, options, versions, factory}},
      message_{this},
      socket_{std::move(socket),
              detail::tcpContext<SocketType>(engine, securityId)} {
  setFlags(securityId, options);
}

template <class SocketType>
TCP_Connection<SocketType>::~TCP_Connection() {
  channelDown();

  // Check that secure socket was shutdown correctly.
  SecurityCheck::beforeClose(this, socket_.next_layer().native_handle());

  log_info("Close TCP connection", std::make_pair("connid", connectionId()));
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
  log_debug("TCP_Connection::flush started",
            std::make_pair("connid", connectionId()));
  auto self(this->shared_from_this());
  socket_.buf_flush(connectionId(), [this, self](const std::error_code &error) {
    log_debug("TCP_Connection::flush finished",
              std::make_pair("connid", connectionId()), error);
    if (error) {
      log_error("TCP_Connection::flush error", error);
      // FIXME(bfish): check for error on close?
      socket_.lowest_layer().close();
    }
  });
}

template <class SocketType>
void TCP_Connection<SocketType>::shutdown(bool reset) {
  if (!(flags() & Connection::kShutdownCalled)) {
    // Do nothing if socket is not open.
    if (!socket_.is_open())
      return;
    setFlags(flags() | Connection::kShutdownCalled);
    // If reset is true, we need to forcefully reset the TCP connection.
    if (reset) {
      log_debug("TCP_Connection::shutdown reset",
                std::make_pair("connid", connectionId()));
      setFlags(flags() | Connection::kShutdownDone);
      channelDown();
      socket_.shutdownLowestLayer(true);
      return;
    }

    log_debug("TCP_Connection::shutdown started",
              std::make_pair("connid", connectionId()));
    auto self(this->shared_from_this());
    socket_.async_shutdown([this, self](const std::error_code &error) {
      setFlags(flags() | Connection::kShutdownDone);
      log_debug("TCP_Connection::shutdown handler called",
                std::make_pair("connid", connectionId()));
      if (error && error != asio::error::eof) {
        log_error("TCP_Connection::shutdown result",
                  std::make_pair("connid", connectionId()), error);
      }
      socket_.shutdownLowestLayer();
    });
  }
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncConnect(
    const IPv6Endpoint &remoteEndpt,
    std::function<void(Channel *, std::error_code)> resultHandler) {
  auto self(this->shared_from_this());
  tcp::endpoint endpt = convertEndpoint<tcp>(remoteEndpt);

  log_info("Initiate TCP connection to", remoteEndpt,
           std::make_pair("connid", connectionId()));

  OFP_BEGIN_IGNORE_PADDING

  socket_.lowest_layer().async_connect(
      endpt, [this, self, resultHandler](const asio::error_code &err) {
        // `async_connect` may not report an error if close() is not called.

        if (!err) {
          assert(socket_.lowest_layer().is_open());

          log_info("Establish TCP connection", localEndpoint(), "-->",
                   remoteEndpoint(), std::make_pair("connid", connectionId()));

          disableNagleAlgorithm();
          asyncHandshake(true);

        } else {
          log_error("TCP connect failed",
                    std::make_pair("connid", connectionId()), err);
        }

        resultHandler(this, err);
      });

  OFP_END_IGNORE_PADDING
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncAccept() {
  // Do nothing if socket is not open.
  if (!socket_.is_open())
    return;

  log_info("Accept TCP connection", localEndpoint(), "<--", remoteEndpoint(),
           std::make_pair("connid", connectionId()));

  disableNagleAlgorithm();
  asyncHandshake(false);
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncReadHeader() {
  // Do nothing if socket is not open.
  if (!socket_.is_open()) {
    log_debug("asyncReadHeader called with socket closed",
              std::make_pair("connid", connectionId()));
    return;
  }

  auto self(this->shared_from_this());
  updateTimeReadStarted();

  asio::async_read(
      socket_,
      asio::buffer(message_.mutableDataResized(sizeof(Header)), sizeof(Header)),
          [this, self](const asio::error_code &err, size_t length) {
            log_debug("asyncReadHeader callback",
                      std::make_pair("connid", connectionId()), err);
            if (!err) {
              assert(length == sizeof(Header));
              const Header *hdr = message_.header();

              UInt8 negotiatedVersion =
                  (flags() & kPermitsOtherVersions) ? 0 : version();

              if (hdr->validateInput(negotiatedVersion)) {
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
                log_debug("asyncReadHeader header validation failed",
                          std::make_pair("connid", connectionId()));
                channelDown();
                engine()->alert(this, "Invalid OpenFlow message header",
                                {hdr, sizeof(*hdr)});
              }

            } else {
              assert(err);

              if (err != asio::error::eof &&
                  err != asio::error::operation_aborted &&
                  err != asio::error::connection_reset) {
                log_error("asyncReadHeader error",
                          std::make_pair("connid", connectionId()), err);
              }

              channelDown();
              shutdown();
            }
          });
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncReadMessage(size_t msgLength) {
  assert(msgLength > sizeof(Header));

  // Do nothing if socket is not open.
  if (!socket_.is_open()) {
    log_debug("asyncReadMessage called with socket closed",
              std::make_pair("connid", connectionId()));
    return;
  }

  auto self(this->shared_from_this());
  updateTimeReadStarted();

  asio::async_read(
      socket_,
      asio::buffer(message_.mutableDataResized(msgLength) + sizeof(Header),
                   msgLength - sizeof(Header)),
          [this, self](const asio::error_code &err, size_t bytes_transferred) {
            log_debug("asyncReadMessage callback",
                      std::make_pair("connid", connectionId()), err);
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
                log_info("asyncReadMessage error",
                         std::make_pair("connid", connectionId()), err);
              }
              channelDown();
            }
          });
}

template <class SocketType>
void TCP_Connection<SocketType>::asyncHandshake(bool isClient) {
  // Indicate the connection is up.
  setFlags(flags() | kConnectionUp);

  // Start async handshake.
  auto mode = isClient ? asio::ssl::stream_base::client
                       : asio::ssl::stream_base::server;

  // Set up verify callback.
  SecurityCheck::beforeHandshake(this, socket_.next_layer().native_handle(),
                                 isClient);

  OFP_BEGIN_IGNORE_PADDING

  auto self(this->shared_from_this());
  socket_.async_handshake(mode, [this, self](const asio::error_code &err) {
    SecurityCheck::afterHandshake(this, socket_.next_layer().native_handle(),
                                  err);

    if (!err) {
      channelUp();
      asyncReadHeader();
    }
  });

  OFP_END_IGNORE_PADDING
}

template <class SocketType>
void TCP_Connection<SocketType>::disableNagleAlgorithm() {
  // We always send and receive complete messages; disable Nagle algorithm.
  std::error_code err;
  socket_.lowest_layer().set_option(tcp::no_delay(true), err);

  if (err) {
    log_error("TCP_Connection::disableNagleAlgorithm", err);
  }
}

}  // namespace sys
}  // namespace ofp
