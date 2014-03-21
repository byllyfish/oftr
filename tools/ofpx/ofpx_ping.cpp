// The following two definitions are required by llvm/Support/DataTypes.h
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1

#include "llvm/Support/CommandLine.h"
#include "ofpx.h"
#include "ofp/ofp.h"
#include <iostream>

using namespace ofp;

// Maximum size of ping message payload is maximum length of OpenFlow message 
// minus header size.

const size_t kMaxPingData = OFP_MAX_SIZE - 8;

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

//-------------------------//
// p i n g _ c o n n e c t //
//-------------------------//

static int ping_connect(const IPv6Endpoint &endpt, int size) {
  Driver driver;

  if (size < 0 || size > kMaxPingData) {
    size = kMaxPingData;
  }

  ByteList echoData = ByteList::iota(size);

  auto exc =
      driver.connect(Driver::Bridge, endpt, ProtocolVersions::All,
                     [echoData]() { return new PingListener{&echoData}; });

  int exitCode = 0;
  exc.done([&exitCode](const std::error_code &err) {
    if (err) {
      std::cerr << "ERROR: " << err << '\n';
      exitCode = 2;
    }
  });

  driver.run();

  return exitCode;
}

using namespace llvm;

struct IPv6EndpointParser : public cl::parser<IPv6Endpoint> {
public:
  // parse - Return true on error.
  bool parse(cl::Option &O, StringRef ArgName, StringRef ArgValue,
             IPv6Endpoint &Val) {
    if (Val.parse(ArgValue))
      return false;
    return O.error("Unexpected endpoint format '" + ArgValue + "'!");
  }
};

//-------------------//
// o f p x _ p i n g //
//-------------------//

int ofpx_ping(int argc, char **argv) {

  cl::opt<IPv6Endpoint, false, IPv6EndpointParser> connect{
      "connect",                  cl::desc("where to connect"),
      cl::value_desc("endpoint"), cl::ValueRequired};

  cl::opt<int> size{"size", cl::desc("data size"), 
      cl::value_desc("size")};

  cl::ParseCommandLineOptions(argc, argv);

  return ping_connect(connect, size);
}
