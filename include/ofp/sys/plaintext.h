#ifndef OFP_SYS_PLAINTEXT_H
#define OFP_SYS_PLAINTEXT_H

#include "ofp/sys/boost_asio.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

/// \brief Encapsulates a stream with an identical interface to ssl::stream, 
/// so the two classes can be transparently interchanged. That is, 
/// sys::Plaintext<tcp::socket> is the plaintext equivalent of
/// ssl::stream<tcp::socket>; the plaintext version does a no-op handshake
/// and shutdown.

template <class StreamType>
class Plaintext : private StreamType, public asio::ssl::stream_base {
  using inherited = StreamType;

public:
  using next_layer_type = inherited;
  using lowest_layer_type = typename inherited::lowest_layer_type;

  // Inherit constructors (for now).
  using inherited::inherited;

  using inherited::get_io_service;
  using inherited::async_read_some;
  using inherited::async_write_some;
  using inherited::lowest_layer;

  // using inherited::async_connect;
  // using inherited::remote_endpoint;
  // using inherited::is_open;
  // using inherited::set_option;
  // using inherited::close;

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
};

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_PLAINTEXT_H
