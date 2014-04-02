//  ===== ---- ofp/queuegetconfigreply.h -------------------*- C++ -*- =====  //
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
/// \brief Defines the QueueGetConfigReply and QueueGetConfigReplyBuilder
/// classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_QUEUEGETCONFIGREPLY_H_
#define OFP_QUEUEGETCONFIGREPLY_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/queuelist.h"

namespace ofp {

// Note: This message is replaced by a MultipartReply in version 1.4.

class QueueGetConfigReply
    : public ProtocolMsg<QueueGetConfigReply, OFPT_QUEUE_GET_CONFIG_REPLY> {
public:
  UInt32 port() const { return port_; }
  QueueRange queues() const;

  bool validateInput(size_t length) const;

private:
  Header header_;
  Big32 port_;
  Padding<4> pad_;

  QueueGetConfigReply() : header_{type()} {}

  friend class QueueGetConfigReplyBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(QueueGetConfigReply) == 16, "Unexpected size.");
static_assert(IsStandardLayout<QueueGetConfigReply>(),
              "Expected standard layout.");
static_assert(IsTriviallyCopyable<QueueGetConfigReply>(),
              "Expected trivially copyable.");

class QueueGetConfigReplyBuilder {
public:
  QueueGetConfigReplyBuilder() = default;

  void setPort(UInt32 port) { msg_.port_ = port; }

  void setQueues(const QueueRange &queues) { queues_.assign(queues); }
  void setQueues(const QueueList &queues) { setQueues(queues.toRange()); }

  UInt32 send(Writable *channel);

private:
  QueueGetConfigReply msg_;
  QueueList queues_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_QUEUEGETCONFIGREPLY_H_
