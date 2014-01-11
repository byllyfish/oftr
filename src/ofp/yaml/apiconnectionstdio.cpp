//  ===== ---- ofp/yaml/apiconnectionstdio.cpp -------------*- C++ -*- =====  //
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
/// \brief Implements the yaml::ApiConnectionStdio class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/yaml/apiconnectionstdio.h"
#include "ofp/sys/boost_asio.h"

using namespace ofp::yaml;
using namespace ofp::sys;

ApiConnectionStdio::ApiConnectionStdio(ApiServer *server,
                                       asio::posix::stream_descriptor input,
                                       asio::posix::stream_descriptor output,
                                       bool listening)
    : ApiConnection{server, listening}, input_{std::move(input)},
      output_{std::move(output)} {}

void ApiConnectionStdio::setInput(int input) {
  assert(input >= 0);

  input_.assign(input);
}

void ApiConnectionStdio::setOutput(int output) {
  assert(output >= 0);

  output_.assign(output);
}

void ApiConnectionStdio::write(const std::string &msg) {
  outgoing_[outgoingIdx_].add(msg.data(), msg.length());
  if (!writing_) {
    asyncWrite();
  }
}

void ApiConnectionStdio::asyncAccept() {
  // Start first async read.
  asyncRead();
}

void ApiConnectionStdio::asyncRead() {
  auto self(shared_from_this());

  asio::async_read_until(
      input_, streambuf_, '\n',
      [this, self](const asio::error_code &err, size_t bytes_transferred) {
        if (!err) {
          std::istream is(&streambuf_);
          std::string line;
          std::getline(is, line);
          handleInputLine(&line);
          asyncRead();
        } else if (err != asio::error::eof) {
          auto exc = makeException(err);
          log::info("ApiConnection::asyncRead err", exc);
        }
      });
}

void ApiConnectionStdio::asyncWrite() {
  assert(!writing_);

  int idx = outgoingIdx_;
  outgoingIdx_ = !outgoingIdx_;
  writing_ = true;

  const UInt8 *data = outgoing_[idx].data();
  size_t size = outgoing_[idx].size();

  auto self(shared_from_this());

  asio::async_write(
      output_, asio::buffer(data, size),
      [this, self](const asio::error_code &err, size_t bytes_transferred) {

        if (!err) {
          assert(bytes_transferred == outgoing_[!outgoingIdx_].size());

          writing_ = false;
          outgoing_[!outgoingIdx_].clear();
          if (outgoing_[outgoingIdx_].size() > 0) {
            // Start another async write for the other output buffer.
            asyncWrite();
          }

        } else {
          log::debug("Write error ", makeException(err));
        }
      });
}
