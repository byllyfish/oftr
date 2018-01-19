// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ECHOREQUEST_H_
#define OFP_ECHOREQUEST_H_

#include "ofp/bytelist.h"
#include "ofp/protocolmsg.h"

namespace ofp {

class EchoRequest
    : public ProtocolMsg<EchoRequest, OFPT_ECHO_REQUEST, 8, 65535, false> {
 public:
  static const UInt32 kKeepAliveXID = 0xFBD0FF86;
  static const ByteRange kKeepAliveData;
  static const ByteRange kPassThruData;

  ByteRange echoData() const {
    return SafeByteRange(this, header_.length(), sizeof(Header));
  }
  bool isPassThru() const;

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;

  // Only EchoRequestBuilder can construct an instance.
  EchoRequest() : header_{type()} {}

  friend class EchoRequestBuilder;
};

class EchoRequestBuilder {
 public:
  explicit EchoRequestBuilder(UInt32 xid = 0) { msg_.header_.setXid(xid); }

  void setEchoData(const void *data, size_t length) { data_.set(data, length); }
  void setKeepAlive();

  UInt32 send(Writable *channel);

 private:
  EchoRequest msg_;
  ByteList data_;
};

}  // namespace ofp

#endif  // OFP_ECHOREQUEST_H_
