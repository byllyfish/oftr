//  ===== ---- ofp/api/apiconnectionstdio.h ----------------*- C++ -*- =====  //
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
/// \brief Defines the ApiConnectionStdio class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_API_APICONNECTIONSTDIO_H_
#define OFP_API_APICONNECTIONSTDIO_H_

#include "ofp/sys/asio_utils.h"
#include "ofp/api/apiconnection.h"

namespace ofp {
namespace api {

OFP_BEGIN_IGNORE_PADDING

class ApiConnectionStdio : public ApiConnection {
 public:
  ApiConnectionStdio(ApiServer *server, asio::posix::stream_descriptor input,
                     asio::posix::stream_descriptor output);

  void setInput(int input);
  void setOutput(int output);

  void asyncAccept() override;

 protected:
  void write(const std::string &msg) override;
  void asyncRead() override;
  void asyncWrite();

 private:
  asio::posix::stream_descriptor input_;
  asio::posix::stream_descriptor output_;
  asio::streambuf streambuf_;

  // Use a two buffer strategy for async-writes. We queue up data in one
  // buffer while we're in the process of writing the other buffer.
  ByteList outgoing_[2];
  int outgoingIdx_ = 0;
  bool writing_ = false;
};

OFP_END_IGNORE_PADDING

}  // namespace api
}  // namespace ofp

#endif  // OFP_API_APICONNECTIONSTDIO_H_
