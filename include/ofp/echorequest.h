//  ===== ---- ofp/echorequest.h ---------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the EchoRequest and EchoRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ECHOREQUEST_H
#define OFP_ECHOREQUEST_H

#include "ofp/protocolmsg.h"
#include "ofp/bytelist.h"

namespace ofp { // <namespace ofp>

class EchoRequest
    : public ProtocolMsg<EchoRequest, OFPT_ECHO_REQUEST, 8, 65535, false> {
public:
  ByteRange echoData() const;

  bool validateLength(size_t length) const;

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

} // </namespace ofp>

#endif // OFP_ECHOREQUEST_H
