// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_PLAINTEXT_ADAPTER_H_
#define OFP_SYS_PLAINTEXT_ADAPTER_H_

#include "ofp/sys/asio_utils.h"

namespace ofp {
namespace sys {

class Plaintext_Adapter {
 public:
  typedef void (*DeliverFunc)(const void *data, size_t length, void *userData);

  Plaintext_Adapter(SSL_CTX *ctx, DeliverFunc sendCallback,
                    DeliverFunc receiveCallback, void *userData)
      : sendCallback_{sendCallback},
        receiveCallback_{receiveCallback},
        userData_{userData} {}
  ~Plaintext_Adapter() {}

  void *native_handle() { return nullptr; }

  void connect() {}
  void accept() {}

  bool isHandshakeDone() const { return false; }

  void sendDatagram(const void *datagram, size_t length) {
    sendCallback_(datagram, length, userData_);
  }
  void datagramReceived(const void *datagram, size_t length) {
    receiveCallback_(datagram, length, userData_);
  }
  void pollIdle() {}

 private:
  DeliverFunc sendCallback_;
  DeliverFunc receiveCallback_;
  void *userData_;
};

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_PLAINTEXT_ADAPTER_H_
