// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_ECHOREPLY_H_
#define OFP_ECHOREPLY_H_

#include "ofp/protocolmsg.h"
#include "ofp/bytelist.h"

namespace ofp {

class EchoReply
    : public ProtocolMsg<EchoReply, OFPT_ECHO_REPLY, 8, 65535, false> {
 public:
  ByteRange echoData() const;

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

  void send(Writable *channel);

 private:
  EchoReply msg_;
  ByteList data_;
};

}  // namespace ofp

#endif  // OFP_ECHOREPLY_H_
