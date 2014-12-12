// Copyright 2014-present Bill Fisher. All rights reserved.

#include "llvm/Support/CommandLine.h"
#include "ofpx_ping.h"
#include <iostream>

using namespace ofp;
using namespace ofpx;

//-------------------------//
// P i n g L i s t e n e r //
//-------------------------//

OFP_BEGIN_IGNORE_PADDING

class PingListener : public ChannelListener {
 public:
  explicit PingListener(const ByteList *echoData) : echoData_{echoData} {}

  void onChannelUp(Channel *channel) override {
    channel_ = channel;
    sendPing();
  }

  void onChannelDown(Channel *channel) override {}

  void onMessage(const Message *message) override {
    if (message->type() == OFPT_ECHO_REPLY) {
      onPingReply(message);
    } else {
      onOther(message);
    }
  }

 private:
  Channel *channel_ = nullptr;
  UInt32 lastXid_ = 0;
  const ByteList *echoData_;

  void onPingReply(const Message *message) {
    const EchoReply *reply = EchoReply::cast(message);
    if (reply) {
      // Check valid ping reply.
    } else {
      // Handle malformed ping reply.
    }

    // Send next ping!
    sendPing();
  }

  void onOther(const Message *message) {
    // Handle other message type.
  }

  void sendPing() {
    EchoRequestBuilder echoRequest;
    echoRequest.setEchoData(echoData_->data(), echoData_->size());
    lastXid_ = echoRequest.send(channel_);
  }
};

OFP_END_IGNORE_PADDING

//-------//
// r u n //
//-------//

int Ping::run(int argc, const char * const *argv) {
  cl::ParseCommandLineOptions(argc, argv);
  return ping();
}

//---------//
// p i n g //
//---------//

int Ping::ping() {
  int exitCode = 0;

  int size = size_;
  if (size < 0 || size > kMaxPingData) {
    size = kMaxPingData;
  }

  ByteList echoData = ByteList::iota(Unsigned_cast(size));
  Driver driver;

  (void)driver.connect(ChannelMode::Raw, 0, endpoint_, ProtocolVersions::All,
                       [echoData]() { return new PingListener{&echoData}; },
                       [&exitCode](Channel *, std::error_code err) {
                         if (err) {
                           std::cerr << "ERROR: " << err << '\n';
                           exitCode = 2;
                         }
                       });

  driver.run();

  return exitCode;
}
