//  ===== ---- ofp/yaml/apiconnectiontcp.cpp ---------------*- C++ -*- =====  //
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
/// \brief Implements the ApiConnectionTCP class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/yaml/apiconnectiontcp.h"

using namespace ofp::yaml;
using namespace ofp::sys;

ApiConnectionTCP::ApiConnectionTCP(ApiServer *server, sys::tcp::socket socket,
                                   bool listening)
    : ApiConnection{server, listening}, socket_{std::move(socket)} {}

void ApiConnectionTCP::write(const std::string &msg) {
  outgoing_[outgoingIdx_].add(msg.data(), msg.length());
  if (!writing_) {
    asyncWrite();
  }
}

void ApiConnectionTCP::asyncAccept() {
  // Do nothing if socket is not open.
  if (!socket_.is_open())
    return;

  // We always send and receive complete messages; disable Nagle algorithm.
  socket_.set_option(tcp::no_delay(true));

  // Start first async read.
  asyncRead();
}

void ApiConnectionTCP::asyncRead() {
  auto self(shared_from_this());

  boost::asio::async_read_until(
      socket_, streambuf_, '\n',
      [this, self](const error_code & err, size_t bytes_transferred) {
        if (!err) {
          std::istream is(&streambuf_);
          std::string line;
          std::getline(is, line);
          handleInputLine(&line);
          asyncRead();
        } else if (!isAsioEOF(err)) {
          auto exc = makeException(err);
          log::info("ApiConnectionTCP::asyncRead err", exc);
        }
      });
}

void ApiConnectionTCP::asyncWrite() {
  assert(!writing_);

  int idx = outgoingIdx_;
  outgoingIdx_ = !outgoingIdx_;
  writing_ = true;

  const UInt8 *data = outgoing_[idx].data();
  size_t size = outgoing_[idx].size();

  auto self(shared_from_this());

  boost::asio::async_write(
      socket_, boost::asio::buffer(data, size),
      [this, self](const error_code & err, size_t bytes_transferred) {

        if (!err) {
          assert(bytes_transferred == outgoing_[!outgoingIdx_].size());

          writing_ = false;
          outgoing_[!outgoingIdx_].clear();
          if (outgoing_[outgoingIdx_].size() > 0) {
            // Start another async write for the other output buffer.
            asyncWrite();
          }

        } else {
          auto exc = makeException(err);
          log::debug("ApiConnectionTCP::async_write err", exc);
        }
      });
}
