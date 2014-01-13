#ifndef OFP_SYS_BUFFERED_H
#define OFP_SYS_BUFFERED_H

#include "ofp/sys/boost_asio.h"
#include "ofp/bytelist.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

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
      log::debug("Write error ", makeException(err));
    }
  });
}

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_BUFFERED_H
