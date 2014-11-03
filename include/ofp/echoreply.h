//  ===== ---- ofp/echoreply.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the EchoReply and EchoReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

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
