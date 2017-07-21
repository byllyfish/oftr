// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ECHOREPLY_H_
#define OFP_ECHOREPLY_H_

#include "ofp/bytelist.h"
#include "ofp/protocolmsg.h"

namespace ofp {

class EchoReply
    : public ProtocolMsg<EchoReply, OFPT_ECHO_REPLY, 8, 65535, false> {
 public:
  ByteRange echoData() const { return SafeByteRange(this, header_.length(), sizeof(Header)); }
  bool isKeepAlive() const;

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;

  // Only EchoReplyBuilder can construct an instance.
  EchoReply() : header_{type()} {}

  friend class EchoReplyBuilder;
};

class EchoReplyBuilder {
 public:
  explicit EchoReplyBuilder(UInt32 xid);

  void setEchoData(const void *data, size_t length) { data_.set(data, length); }
  void setKeepAlive();

  void send(Writable *channel);

 private:
  EchoReply msg_;
  ByteList data_;
};

}  // namespace ofp

#endif  // OFP_ECHOREPLY_H_
