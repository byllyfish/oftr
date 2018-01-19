// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_MESSAGECONVERT_H_
#define OFP_DEMUX_MESSAGECONVERT_H_

#include "ofp/byterange.h"
#include "ofp/timestamp.h"

namespace ofp {

class Message;

namespace demux {

class PktSource;

/// Convert each captured packet to a PacketIn message.

class MessageConvert {
 public:
  using MessageCallback = void (*)(Message *, void *);

  MessageConvert(MessageCallback callback, void *context)
      : callback_{callback}, context_{context} {}

  void runLoop(PktSource *pcap);

  void submitEthernet(Timestamp ts, ByteRange capture, unsigned len);

 private:
  MessageCallback callback_;
  void *context_ = nullptr;
};

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_MESSAGECONVERT_H_
