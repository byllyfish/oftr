// Copyright 2014-present Bill Fisher. All rights reserved.

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
