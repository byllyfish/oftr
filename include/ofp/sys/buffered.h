//  ===== ---- ofp/sys/buffered.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the sys::Buffered class for use by ASIO streams.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_BUFFERED_H_
#define OFP_SYS_BUFFERED_H_

#include "ofp/sys/asio_utils.h"
#include "ofp/bytelist.h"

namespace ofp {
namespace sys {

OFP_BEGIN_IGNORE_PADDING

template <class StreamType>
class Buffered : private StreamType {
  using inherited = StreamType;

public:
  using next_layer_type = inherited;
  using lowest_layer_type = typename inherited::lowest_layer_type;

  using inherited::inherited;

  Buffered(tcp::socket sock, asio::ssl::context &context)
    : inherited{sock.get_io_service(), context} {
      lowest_layer() = std::move(sock);
  }

  using inherited::get_io_service;
  using inherited::async_read_some;
  // using inherited::async_write_some;
  using inherited::lowest_layer;
  using inherited::async_handshake;
  using inherited::async_shutdown;

  // using inherited::async_connect;
  // using inherited::remote_endpoint;
  // using inherited::is_open;
  // using inherited::set_option;
  // using inherited::close;

  ~Buffered() {
    // Close the underlying socket before destroying buffers.
    lowest_layer().close();
  }

  const next_layer_type &next_layer() const { return *this; }
  next_layer_type &next_layer() { return *this; }

  void buf_write(const void *data, size_t length) {
    buffer_[bufferIdx_].add(data, length);
  }

  void buf_flush();

private:
  // Use a two buffer strategy for async-writes. We queue up data in one
  // growable buffer while we're in the process of flushing the other buffer.
  ByteList buffer_[2];
  int bufferIdx_ = 0;
  bool isFlushing_ = false;
};

OFP_END_IGNORE_PADDING

template <class StreamType>
void Buffered<StreamType>::buf_flush() {
  ByteList &outgoing = buffer_[bufferIdx_];
  if (isFlushing_ || outgoing.size() == 0)
    return;

  bufferIdx_ = !bufferIdx_;
  isFlushing_ = true;

  log::trace("write", outgoing.data(), outgoing.size());

  async_write(next_layer(), asio::buffer(outgoing.data(), outgoing.size()),
              [this](const asio::error_code &err, size_t bytes_transferred) {

    if (!err) {
      assert(bytes_transferred == buffer_[!bufferIdx_].size());

      isFlushing_ = false;
      buffer_[!bufferIdx_].clear();
      if (buffer_[bufferIdx_].size() > 0) {
        // Start another async write for the other output buffer.
        buf_flush();
      }

    } else {
      log::debug("Write error ", err);
    }
  });
}

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_BUFFERED_H_
