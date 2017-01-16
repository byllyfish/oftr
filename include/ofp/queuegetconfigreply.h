// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_QUEUEGETCONFIGREPLY_H_
#define OFP_QUEUEGETCONFIGREPLY_H_

#include "ofp/padding.h"
#include "ofp/portnumber.h"
#include "ofp/protocolmsg.h"
#include "ofp/queuelist.h"

namespace ofp {

// Note: This message is replaced by a MultipartReply in version 1.4.

class QueueGetConfigReply
    : public ProtocolMsg<QueueGetConfigReply, OFPT_QUEUE_GET_CONFIG_REPLY, 16> {
 public:
  PortNumber port() const { return port_; }
  QueueRange queues() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  PortNumber port_;
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

  void setPort(PortNumber port) { msg_.port_ = port; }

  void setQueues(const QueueRange &queues) { queues_.assign(queues); }
  void setQueues(const QueueList &queues) { setQueues(queues.toRange()); }

  UInt32 send(Writable *channel);

 private:
  QueueGetConfigReply msg_;
  QueueList queues_;

  UInt32 sendV1(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_QUEUEGETCONFIGREPLY_H_
