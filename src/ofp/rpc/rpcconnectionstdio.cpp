// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcconnectionstdio.h"
#include "ofp/rpc/rpcevents.h"
#include "ofp/sys/asio_utils.h"

using ofp::rpc::RpcConnectionStdio;

// The Stdio Text API uses UTF-8 and has JSON events delimited by '\n'. White
// space characters (SPACE, HT, VT, FF, CR) are permitted. All other control
// characters are reserved.

const char RPC_EVENT_DELIMITER_CHAR = '\n';

RpcConnectionStdio::RpcConnectionStdio(RpcServer *server,
                                       asio::posix::stream_descriptor input,
                                       asio::posix::stream_descriptor output)
    : RpcConnection{server},
      input_{std::move(input)},
      output_{std::move(output)},
      streambuf_{RPC_MAX_MESSAGE_SIZE} {}

void RpcConnectionStdio::write(const std::string &msg) {
  outgoing_[outgoingIdx_].add(msg.data(), msg.length());
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
  asyncRead();
}

void RpcConnectionStdio::asyncRead() {
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
          asyncRead();
        } else if (err == asio::error::not_found) {
          log_error("RpcConnectionStdio::asyncRead: input too large",
                    RPC_MAX_MESSAGE_SIZE, err);
          rpcRequestTooBig();
        } else if (err != asio::error::eof &&
                   err != asio::error::operation_aborted) {
          log_error("RpcConnectionStdio::asyncRead error", err);
        }
      });
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
