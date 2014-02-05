//  ===== ---- ofp/queuegetconfigrequest.h -----------------*- C++ -*- =====  //
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
/// \brief Defines the QueueGetConfigRequest and QueueGetConfigRequestBuilder
/// classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_QUEUEGETCONFIGREQUEST_H_
#define OFP_QUEUEGETCONFIGREQUEST_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"

namespace ofp {

// Note: This message is replaced by a MultipartRequest in version 1.4.

class QueueGetConfigRequest
    : public ProtocolMsg<QueueGetConfigRequest, OFPT_QUEUE_GET_CONFIG_REQUEST> {
public:
  UInt32 port() const { return port_; }

  bool validateLength(size_t length) const;

private:
  Header header_;
  Big32 port_;
  Padding<4> pad_;

  QueueGetConfigRequest() : header_{type()} {}

  friend class QueueGetConfigRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(QueueGetConfigRequest) == 16, "Unexpected size.");
static_assert(IsStandardLayout<QueueGetConfigRequest>(),
              "Expected standard layout.");
static_assert(IsTriviallyCopyable<QueueGetConfigRequest>(),
              "Expected trivially copyable.");

class QueueGetConfigRequestBuilder {
public:
  QueueGetConfigRequestBuilder() = default;
  explicit QueueGetConfigRequestBuilder(const QueueGetConfigRequest *msg);

  void setPort(UInt32 port) { msg_.port_ = port; }

  UInt32 send(Writable *channel);

private:
  QueueGetConfigRequest msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_QUEUEGETCONFIGREQUEST_H_
