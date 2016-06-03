// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_MESSAGESOURCE_H_
#define OFP_DEMUX_MESSAGESOURCE_H_

#include "ofp/byterange.h"
#include "ofp/demux/flowcache.h"
#include "ofp/ipv6endpoint.h"
#include "ofp/timestamp.h"

namespace ofp {

class Message;

namespace demux {

class PktSource;

/// \brief Given captured packets, reassemble annotated OpenFlow messages.
///
/// - Does not support fragmented TCP packets.

OFP_BEGIN_IGNORE_PADDING

class MessageSource {
 public:
  using MessageCallback = void (*)(Message *, void *);

  explicit MessageSource(MessageCallback callback, void *context,
                         const std::string &outputDir, bool skipPayload = false,
                         size_t maxMissingBytes = 0)
      : callback_{callback},
        context_{context},
        outputDir_{outputDir},
        maxMissingBytes_{maxMissingBytes},
        skipPayload_{skipPayload} {}

  void runLoop(PktSource *pcap);

  void submitEthernet(Timestamp ts, ByteRange capture);
  void submitIP(Timestamp ts, ByteRange capture);
  void finish();

 private:
  MessageCallback callback_ = nullptr;
  void *context_ = nullptr;
  Timestamp ts_;
  IPv6Endpoint src_;
  IPv6Endpoint dst_;
  UInt32 seq_ = 0;
  UInt16 flags_ = 0;
  FlowCache flows_;
  std::string outputDir_;
  size_t maxMissingBytes_;
  bool skipPayload_;

  void submitEthernet(const UInt8 *data, size_t length);
  void submitIPv4(const UInt8 *data, size_t length);
  void submitIPv6(const UInt8 *data, size_t length);
  void submitTCP(const UInt8 *data, size_t length);
  void submitUDP(const UInt8 *data, size_t length);

  size_t submitPayload(const UInt8 *data, size_t length, UInt64 sessionID);
  void deliverMessage(const UInt8 *data, size_t length, UInt64 sessionID);

  bool hasOutputDir() const { return !outputDir_.empty(); }
  void outputWrite(const IPv6Endpoint &src, const IPv6Endpoint &dst,
                   const FlowData &flow, size_t n);
};

OFP_END_IGNORE_PADDING

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_MESSAGESOURCE_H_
