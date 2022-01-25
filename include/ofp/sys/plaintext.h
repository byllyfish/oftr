// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_PLAINTEXT_H_
#define OFP_SYS_PLAINTEXT_H_

#include "ofp/sys/asio_utils.h"

#if !LIBOFP_ENABLE_OPENSSL
// Use asio::ssl::stream_base for handshake_type enum.
#include <asio/ssl/stream_base.hpp>
#endif

namespace ofp {
namespace sys {

/// \brief Encapsulates a stream with an identical interface to ssl::stream,
/// so the two classes can be transparently interchanged. That is,
/// sys::Plaintext<tcp::socket> is the plaintext equivalent of
/// ssl::stream<tcp::socket>; the plaintext version does a no-op handshake
/// and shutdown. This class ignores the ssl::context state.

template <class StreamType>
class Plaintext : private StreamType, public asio::ssl::stream_base {
  using inherited = StreamType;

 public:
  using next_layer_type = inherited;
  using lowest_layer_type = typename inherited::lowest_layer_type;
  using executor_type = typename inherited::executor_type;
  
  // Inherit constructors.
  using inherited::inherited;

  // Add a constructor to take the ssl::context and ignore it.
  template <typename Arg>
  Plaintext(Arg &&arg, asio::ssl::context &ctx)
      : next_layer_type(ASIO_MOVE_CAST(Arg)(arg)) {}

  using inherited::async_read_some;
  using inherited::async_write_some;
  using inherited::get_executor;
  using inherited::lowest_layer;

  // Require clients to use lowest_layer for these methods:
  //   - async_connect
  //   - remote_endpoint
  //   - is_open
  //   - set_option
  //   - close

  const next_layer_type &next_layer() const { return *this; }
  next_layer_type &next_layer() { return *this; }

  template <typename HandshakeHandler>
  void async_handshake(handshake_type type, HandshakeHandler &&handler) {
    // Handshake immediately succeeds.
    handler(asio::error_code{});
  }

  template <typename ShutdownHandler>
  void async_shutdown(ShutdownHandler &&handler) {
    // Shutdown immediately succeeds.
    handler(asio::error_code{});
  }

  void *native_handle() const { return nullptr; }
};

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_PLAINTEXT_H_
