// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_BUFFERED_H_
#define OFP_SYS_BUFFERED_H_

#include "ofp/bytelist.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/handler_allocator.h"

namespace ofp {
namespace sys {

OFP_BEGIN_IGNORE_PADDING

template <class StreamType>
class Buffered : private StreamType {
  using inherited = StreamType;

 public:
  using next_layer_type = inherited;
  using lowest_layer_type = typename inherited::lowest_layer_type;

  // using inherited::inherited;

  Buffered(asio::io_context &io, asio::ssl::context *context)
      : inherited{io, *context} {}

  Buffered(tcp::socket sock, asio::ssl::context *context)
      : inherited{sock.get_executor().context(), *context} {
    lowest_layer() = std::move(sock);
  }

  using inherited::get_executor;
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

  const next_layer_type &next_layer() const { return *this; }
  next_layer_type &next_layer() { return *this; }

  bool is_open() const { return lowest_layer().is_open(); }

  void buf_write(const void *data, size_t length) {
    buffer_[bufferIdx_].add(data, length);
  }

  template <class CompletionHandler>
  void buf_flush(UInt64 id, CompletionHandler &&handler);

  void shutdownLowestLayer();

 private:
  // Use a two buffer strategy for async-writes. We queue up data in one
  // growable buffer while we're in the process of flushing the other buffer.
  ByteList buffer_[2];
  handler_allocator allocator_;
  int bufferIdx_ = 0;
  bool isFlushing_ = false;
};

OFP_END_IGNORE_PADDING

template <class StreamType>
template <class CompletionHandler>
void Buffered<StreamType>::buf_flush(UInt64 id, CompletionHandler &&handler) {
  const ByteList &outgoing = buffer_[bufferIdx_];
  if (isFlushing_ || outgoing.size() == 0)
    return;

  bufferIdx_ = !bufferIdx_;
  isFlushing_ = true;

  log::trace_msg("Write", id, outgoing.data(), outgoing.size());

  async_write(next_layer(), asio::buffer(outgoing.data(), outgoing.size()),
              make_custom_alloc_handler(
                  allocator_, [this, id, handler](const asio::error_code &err,
                                                  size_t bytes_transferred) {
                    log_debug("Buffered::buf_flush handler called",
                               bytes_transferred, std::make_pair("connid", id));
                    if (!err) {
                      assert(bytes_transferred == buffer_[!bufferIdx_].size());

                      isFlushing_ = false;
                      buffer_[!bufferIdx_].clear();
                      if (buffer_[bufferIdx_].size() > 0) {
                        // Start another async write for the other output
                        // buffer.
                        buf_flush(id, handler);
                      } else {
                        // Call completion handler.
                        handler(err);
                      }

                    } else {
                      log_error("Buffered::buf_flush error", err);
                      handler(err);
                    }
                  }));
}

template <class StreamType>
void Buffered<StreamType>::shutdownLowestLayer() {
  if (is_open()) {
    std::error_code ignore;
    lowest_layer().shutdown(tcp::socket::shutdown_both, ignore);
    lowest_layer().close(ignore);
  }
}

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_BUFFERED_H_
