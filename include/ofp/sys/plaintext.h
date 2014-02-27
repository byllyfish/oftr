//  ===== ---- ofp/sys/plaintext.h -------------------------*- C++ -*- =====  //
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
/// \brief Defines the sys::Plaintext class for use by ASIO streams.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_PLAINTEXT_H_
#define OFP_SYS_PLAINTEXT_H_

#include "ofp/sys/asio_utils.h"

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

  // Inherit constructors.
  using inherited::inherited;

  // Add a constructor to take the ssl::context and ignore it.
  template <typename Arg>
  Plaintext(Arg &arg, asio::ssl::context &ctx)
      : next_layer_type(arg) {}

  using inherited::get_io_service;
  using inherited::async_read_some;
  using inherited::async_write_some;
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
};

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_PLAINTEXT_H_
