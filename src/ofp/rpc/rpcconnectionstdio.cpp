// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcconnectionstdio.h"
#include <sys/resource.h>
#include <sys/time.h>
#include "ofp/rpc/rpcevents.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/engine.h"
#include "ofp/timestamp.h"

using ofp::rpc::RpcConnectionStdio;

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

// For `OFP.MESSAGE` notification event.
static const llvm::StringRef kMsgPrefix{"{\"params\":", 10};
static const llvm::StringRef kMsgSuffix{",\"method\":\"OFP.MESSAGE\"}", 24};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR

RpcConnectionStdio::RpcConnectionStdio(RpcServer *server,
                                       asio::posix::stream_descriptor input,
                                       asio::posix::stream_descriptor output,
                                       bool binaryProtocol)
    : RpcConnection{server},
      input_{std::move(input)},
      output_{std::move(output)},
      streambuf_{RPC_MAX_MESSAGE_SIZE},
      metricTimer_{server->engine()->io()},
      binaryProtocol_{binaryProtocol} {}

void RpcConnectionStdio::writeEvent(llvm::StringRef msg, bool ofp_message) {
  ++txEvents_;

  size_t msgSize = ofp_message ? msg.size() + 34 : msg.size();
  txBytes_ += msgSize;

  // TODO(bfish): Make sure the outgoing message doesn't exceed MAX msg size.

  if (binaryProtocol_) {
    // Add header.
    size_t hdrlen = msgSize + sizeof(Big32);
    Big32 hdr = UInt32_narrow_cast((hdrlen << 8) | RPC_EVENT_BINARY_TAG);
    outgoing_[outgoingIdx_].add(&hdr, sizeof(hdr));
  }

  if (ofp_message) {
    outgoing_[outgoingIdx_].add(kMsgPrefix.data(), kMsgPrefix.size());
    outgoing_[outgoingIdx_].add(msg.data(), msg.size());
    outgoing_[outgoingIdx_].add(kMsgSuffix.data(), kMsgSuffix.size());
  } else {
    outgoing_[outgoingIdx_].add(msg.data(), msg.size());
  }

  if (!binaryProtocol_) {
    const UInt8 delimiter = RPC_EVENT_DELIMITER_CHAR;
    outgoing_[outgoingIdx_].add(&delimiter, sizeof(delimiter));
  }

  if (!writing_) {
    asyncWrite();
  }
}

void RpcConnectionStdio::close() {
  input_.close();
  output_.close();
}

void RpcConnectionStdio::asyncAccept() {
  // Start first async read.
  if (binaryProtocol_) {
    asyncReadHeader();
  } else {
    asyncReadLine();
  }

  // Start optional metrics timer.
  Milliseconds metricInterval = server_->metricInterval();
  if (metricInterval != 0_ms) {
    asyncMetrics(metricInterval);
  }
}

void RpcConnectionStdio::asyncReadLine() {
  auto self(shared_from_this());

  asio::async_read_until(
      input_, streambuf_, RPC_EVENT_DELIMITER_CHAR,
      [this, self](const asio::error_code &err, size_t bytes_transferred) {
        if (!err) {
          std::istream is(&streambuf_);
          std::string line;
          std::getline(is, line, RPC_EVENT_DELIMITER_CHAR);
          log::trace_rpc("Read RPC", 0, line.data(), line.size());
          handleEvent(line);
          asyncReadLine();
        } else if (err == asio::error::not_found) {
          log_error("RpcConnectionStdio::asyncReadLine: input too large",
                    RPC_MAX_MESSAGE_SIZE, err);
          rpcRequestTooBig();
        } else if (err == asio::error::eof) {
          // Log warning if there are unread bytes in the buffer.
          auto bytesUnread = streambuf_.size();
          if (bytesUnread > 0) {
            log_warning(
                "RpcConnectionStdio::asyncReadLine: unread bytes at eof",
                bytesUnread);
          }
        } else if (err != asio::error::operation_aborted) {
          log_error("RpcConnectionStdio::asyncReadLine error", err);
        }
      });
}

void RpcConnectionStdio::asyncReadHeader() {
  auto self(this->shared_from_this());

  asio::async_read(
      input_, asio::buffer(&hdrBuf_, sizeof(hdrBuf_)),
      make_custom_alloc_handler(
          allocator_, [this, self](const asio::error_code &err, size_t length) {
            log_debug("rpc::asyncReadHeader callback", err);
            if (!err) {
              assert(length == sizeof(hdrBuf_));

              UInt32 len = (hdrBuf_ >> 8);
              if (len >= 4) {
                asyncReadMessage(len - 4);
              } else {
                // Too short...
                log_error("RPC invalid header length:", len);
              }
            } else {
              assert(err);

              log_error("RPC readHeader error", err);
            }
          }));
}

void RpcConnectionStdio::asyncReadMessage(size_t msgLength) {
  auto self(this->shared_from_this());

  log_debug("rpc::asyncReadMessage:", msgLength, "bytes");

  eventBuf_.resize(msgLength);
  asio::async_read(
      input_, asio::buffer(eventBuf_),
      make_custom_alloc_handler(
          allocator_, [this, self](const asio::error_code &err, size_t length) {
            log_debug("rpc::asyncReadMessage callback", err, length);
            if (!err) {
              // assert(length == msgLength);
              assert(eventBuf_.size() == length);

              log::trace_rpc("Read RPC", 0, eventBuf_.data(), eventBuf_.size());
              handleEvent(eventBuf_);
              asyncReadHeader();

            } else {
              assert(err);

              log_error("RPC readMessage error", err);
            }
          }));
}

void RpcConnectionStdio::asyncWrite() {
  assert(!writing_);

  int idx = outgoingIdx_;
  outgoingIdx_ = !outgoingIdx_;
  writing_ = true;

  const UInt8 *data = outgoing_[idx].data();
  size_t size = outgoing_[idx].size();

  auto self(shared_from_this());

  log::trace_rpc("Write RPC", 0, data, size);

  asio::async_write(
      output_, asio::buffer(data, size),
      [this, self](const asio::error_code &err, size_t bytes_transferred) {
        if (!err) {
          assert(bytes_transferred == outgoing_[!outgoingIdx_].size());

          writing_ = false;
          outgoing_[!outgoingIdx_].clear();
          if (!outgoing_[outgoingIdx_].empty()) {
            // Start another async write for the other output buffer.
            asyncWrite();
          }
        }
      });
}

void RpcConnectionStdio::asyncMetrics(Milliseconds interval) {
  logMetrics();

  asio::error_code error;
  metricTimer_.expires_after(interval, error);
  assert(!error);

  metricTimer_.async_wait([this, interval](const asio::error_code &err) {
    if (!err) {
      asyncMetrics(interval);
    }
  });
}

void RpcConnectionStdio::logMetrics() {
  struct rusage usage;
  ::getrusage(RUSAGE_SELF, &usage);

  Timestamp utime(Unsigned_cast(usage.ru_utime.tv_sec),
                  Unsigned_cast(usage.ru_utime.tv_usec * 1000));
  Timestamp stime(Unsigned_cast(usage.ru_stime.tv_sec),
                  Unsigned_cast(usage.ru_stime.tv_usec * 1000));

// Use task_info for "resident_size"?
#if defined(LIBOFP_TARGET_DARWIN)
  int64_t kbytes = usage.ru_maxrss / 1024;  // convert to kbytes
#else
  int64_t kbytes = usage.ru_maxrss;  // already in kbytes
#endif

  // TODO(bfish): Include SO_NREAD and SO_NWRITE?

  log_info("Metrics", txEvents_, rxEvents_, txBytes_, rxBytes_,
           outgoing_[0].size(), outgoing_[1].size(), utime, stime, kbytes);
}
