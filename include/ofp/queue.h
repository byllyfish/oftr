// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_QUEUE_H_
#define OFP_QUEUE_H_

#include "ofp/propertylist.h"
#include "ofp/queueproperty.h"
#include "ofp/portnumber.h"

namespace ofp {

class Writable;

namespace deprecated {
class QueueV1;
}  // namespace deprecated

class Queue : private NonCopyable {
 public:
  // Offset of len_ field.
  enum { ProtocolIteratorSizeOffset = 8, ProtocolIteratorAlignment = 8 };

  UInt32 queueId() const { return queueId_; }
  PortNumber port() const { return port_; }

  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  Big32 queueId_;
  PortNumber port_;
  Big16 len_;
  Padding<6> pad_;

  // Only a QueueBuilder can create an instance.
  Queue() = default;

  friend class QueueRange;
  friend class QueueList;
  friend class QueueBuilder;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(Queue) == 16, "Unexpected size.");
static_assert(IsStandardLayout<Queue>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Queue>(), "Expected trivially copyable.");

class QueueBuilder {
 public:
  QueueBuilder() = default;
  explicit QueueBuilder(const deprecated::QueueV1 &queue);

  void setQueueId(UInt32 queueId) { queue_.queueId_ = queueId; }
  void setPort(PortNumber port) { queue_.port_ = port; }

  void setProperties(const PropertyRange &properties);
  void setProperties(const PropertyList &properties) {
    setProperties(properties.toRange());
  }

 private:
  Queue queue_;
  PropertyList properties_;

  enum { SizeWithoutProperties = sizeof(queue_) };

  friend class QueueList;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

namespace deprecated {

class QueueV1 : private NonCopyable {
 public:
  // Offset of len_ field.
  enum { ProtocolIteratorSizeOffset = 4, ProtocolIteratorAlignment = 8 };

  UInt32 queueId() const { return queueId_; }
  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  Big32 queueId_;
  Big16 len_;
  Padding<2> pad_;

  // Only a QueueV1Builder can create an instance.
  QueueV1() = default;

  friend class QueueV1Builder;
};

static_assert(sizeof(QueueV1) == 8, "Unexpected size.");
static_assert(IsStandardLayout<QueueV1>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<QueueV1>(), "Expected trivially copyable.");

class QueueV1Builder {
 public:
  explicit QueueV1Builder(const Queue &queue);

  void write(Writable *channel);

 private:
  QueueV1 queue_;
  PropertyList properties_;
};

}  // namespace deprecated

}  // namespace ofp

#endif  // OFP_QUEUE_H_
