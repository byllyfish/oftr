// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_QUEUEGETCONFIGREPLY_H_
#define OFP_QUEUEGETCONFIGREPLY_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/queuelist.h"

namespace ofp {

// Note: This message is replaced by a MultipartReply in version 1.4.

class QueueGetConfigReply
    : public ProtocolMsg<QueueGetConfigReply, OFPT_QUEUE_GET_CONFIG_REPLY, 16> {
 public:
  UInt32 port() const { return port_; }
  QueueRange queues() const;

  bool validateInput(Validation *context) const;

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
