// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_MESSAGESOURCE_H_
#define OFP_DEMUX_MESSAGESOURCE_H_

#include "ofp/byterange.h"
#include "ofp/ipv6address.h"
#include "ofp/timestamp.h"

namespace ofp {

class Message;

namespace demux {

/// \brief Given captured packets, reassemble annotated OpenFlow messages.
///
/// - Does not support fragmented TCP packets.

OFP_BEGIN_IGNORE_PADDING

class MessageSource {
 public:
  using MessageCallback = void (*)(Message *, void *);

  explicit MessageSource(MessageCallback callback, void *context) : callback_{callback}, context_{context} {}

  void submitPacket(Timestamp ts, ByteRange capture);
  void close();

 private:
  MessageCallback callback_ = nullptr;
  void *context_ = nullptr;
  Timestamp ts_;
  IPv6Address src_;
  IPv6Address dst_;
  UInt16 srcPort_ = 0;
  UInt16 dstPort_ = 0;
  UInt32 len_ = 0;
  UInt32 seqNum_ = 0;
  UInt32 ackNum_ = 0;
  UInt16 flags_ = 0;

  void submitIPv4(const UInt8 *data, size_t length);
  void submitIPv6(const UInt8 *data, size_t length);
  void submitTCP(const UInt8 *data, size_t length);
  void submitUDP(const UInt8 *data, size_t length);
  void submitPayload(const UInt8 *data, size_t length);

  void deliverMessage(const UInt8 *data, size_t length);
};

OFP_END_IGNORE_PADDING

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_MESSAGESOURCE_H_
