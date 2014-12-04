// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_SYS_DTLS_ADAPTER_H_
#define OFP_SYS_DTLS_ADAPTER_H_

#include <deque>
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/datagram.h"

namespace ofp {
namespace sys {

class DTLS_Adapter {
 public:
  typedef void (*DeliverFunc)(const void *data, size_t length, void *userData);

  DTLS_Adapter(asio::ssl::context *context, DeliverFunc sendCallback,
               DeliverFunc receiveCallback, void *userData);
  ~DTLS_Adapter();

  SSL *native_handle() { return ssl_; }
  void setMTU(size_t mtu);

  void connect();
  void accept();

  bool isHandshakeDone() const;

  void sendDatagram(const void *datagram, size_t length);
  void datagramReceived(const void *datagram, size_t length);
  void pollIdle();

  

 private:
  SSL *ssl_;
  BIO *bio_;
  DeliverFunc sendCallback_;
  DeliverFunc receiveCallback_;
  void *userData_;
  std::deque<Datagram> datagrams_;

  void writeOutput();
  void enqueueDatagram(const void *datagram, size_t length);
  void flushDatagrams();

  void logOutput(const UInt8 *p, size_t length);
};

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_DTLS_ADAPTER_H_
