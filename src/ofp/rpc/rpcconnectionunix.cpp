// Copyright (c) 2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcconnectionunix.h"

#include "ofp/rpc/rpcevents.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/engine.h"
#include "ofp/timestamp.h"

using ofp::rpc::RpcConnectionUnix;

// For `OFP.MESSAGE` notification event.
constexpr llvm::StringLiteral kMsgPrefix{"{\"params\":"};
constexpr llvm::StringLiteral kMsgSuffix{",\"method\":\"OFP.MESSAGE\"}"};

RpcConnectionUnix::RpcConnectionUnix(RpcServer *server,
                                     sys::unix_domain::socket socket,
                                     bool binaryProtocol)
    : RpcConnection{server},
      sock_{std::move(socket)},
      streambuf_{RPC_MAX_MESSAGE_SIZE},
      binaryProtocol_{binaryProtocol} {}

void RpcConnectionUnix::writeEvent(llvm::StringRef msg, bool ofp_message) {
  ++txEvents_;

  size_t msgSize = ofp_message ? msg.size() + 34 : msg.size();
  txBytes_ += msgSize;

  // TODO(bfish): Make sure the outgoing message doesn't exceed MAX msg size.

  if (binaryProtocol_) {
    // Add binary header.
    Big32 hdr = UInt32_narrow_cast((msgSize << 8) | RPC_EVENT_BINARY_TAG);
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

void RpcConnectionUnix::close() {
  sock_.close();
}

void RpcConnectionUnix::asyncAccept() {
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

void RpcConnectionUnix::asyncReadLine() {
  auto self(shared_from_this());

  asio::async_read_until(
      sock_, streambuf_, RPC_EVENT_DELIMITER_CHAR,
      [this, self](const asio::error_code &err, size_t bytes_transferred) {
        if (!err) {
          std::istream is(&streambuf_);
          std::string line;
          std::getline(is, line, RPC_EVENT_DELIMITER_CHAR);
          log::trace_rpc("Read RPC", 0, line.data(), line.size());
          handleEvent(line);
          asyncReadLine();
        } else if (err == asio::error::not_found) {
          // Input line is too big. Send back an error message then allow
          // connection to close.
          log_error("RpcConnectionUnix::asyncReadLine: input too large",
                    RPC_MAX_MESSAGE_SIZE, err);
          rpcRequestInvalid("RPC request is too big");
        } else if (err == asio::error::eof) {
          // Log warning if there are unread bytes in the buffer.
          auto bytesUnread = streambuf_.size();
          if (bytesUnread > 0) {
            log_warning("RpcConnectionUnix::asyncReadLine: unread bytes at eof",
                        bytesUnread);
          }
        } else if (err != asio::error::operation_aborted) {
          // Some error other than operation_aborted occurred. Log the
          // error then allow connection to close.
          log_error("RpcConnectionUnix::asyncReadLine error", err);
        }
      });
}

void RpcConnectionUnix::asyncReadHeader() {
  auto self(this->shared_from_this());

  asio::async_read(
      sock_, asio::buffer(&hdrBuf_, sizeof(hdrBuf_)),
      [this, self](const asio::error_code &err, size_t length) {
            log_debug("rpc::asyncReadHeader callback", err);
            if (!err) {
              assert(length == sizeof(hdrBuf_));

              UInt32 tagLen = hdrBuf_;
              UInt8 tag = tagLen & 0x00FFu;
              UInt32 len = (tagLen >> 8);

              if (tag != RPC_EVENT_BINARY_TAG) {
                log_error("RPC invalid header:", UInt32_cast(tag));
                rpcRequestInvalid("RPC invalid header");
              } else if (len > RPC_MAX_MESSAGE_SIZE) {
                log_error("RPC request is too big:", len);
                rpcRequestInvalid("RPC request is too big");
              } else {
                asyncReadMessage(len);
              }

            } else {
              assert(err);

              log_error("RPC readHeader error", err);
            }
          });
}

void RpcConnectionUnix::asyncReadMessage(size_t msgLength) {
  auto self(this->shared_from_this());

  log_debug("rpc::asyncReadMessage:", msgLength, "bytes");

  eventBuf_.resize(msgLength);
  asio::async_read(
      sock_, asio::buffer(eventBuf_),
      [this, self](const asio::error_code &err, size_t length) {
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
          });
}

void RpcConnectionUnix::asyncWrite() {
  assert(!writing_);

  int idx = outgoingIdx_;
  outgoingIdx_ = !outgoingIdx_;
  writing_ = true;

  const UInt8 *data = outgoing_[idx].data();
  size_t size = outgoing_[idx].size();

  auto self(shared_from_this());

  log::trace_rpc("Write RPC", 0, data, size);

  asio::async_write(
      sock_, asio::buffer(data, size),
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
