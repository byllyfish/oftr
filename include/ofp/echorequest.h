// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_ECHOREQUEST_H_
#define OFP_ECHOREQUEST_H_

#include "ofp/protocolmsg.h"
#include "ofp/bytelist.h"

namespace ofp {

class EchoRequest
    : public ProtocolMsg<EchoRequest, OFPT_ECHO_REQUEST, 8, 65535, false> {
 public:
  ByteRange echoData() const;

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;

  // Only EchoRequestBuilder can construct an instance.
  EchoRequest() : header_{type()} {}

  friend class EchoRequestBuilder;
};

class EchoRequestBuilder {
 public:
  EchoRequestBuilder() = default;

  void setEchoData(const void *data, size_t length) { data_.set(data, length); }

  UInt32 send(Writable *channel);

 private:
  EchoRequest msg_;
  ByteList data_;
};

}  // namespace ofp

#endif  // OFP_ECHOREQUEST_H_
