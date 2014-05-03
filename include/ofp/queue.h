#ifndef OFP_QUEUE_H_
#define OFP_QUEUE_H_

#include "ofp/propertylist.h"
#include "ofp/queueproperty.h"

namespace ofp {

class Queue : private NonCopyable {
 public:
  // Offset of len_ field.
  enum { ProtocolIteratorSizeOffset = 8, ProtocolIteratorAlignment = 8 };

  UInt32 queueId() const { return queueId_; }
  UInt32 port() const { return port_; }

  PropertyRange properties() const;

 private:
  Big32 queueId_;
  Big32 port_;
  Big16 len_ = 0;
  Padding<6> pad_;

  // Only a QueueBuilder can create an instance.
  Queue() = default;

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

  void setQueueId(UInt32 queueId) { queue_.queueId_ = queueId; }
  void setPort(UInt32 port) { queue_.port_ = port; }

  void setProperties(const PropertyRange &properties);
  void setProperties(const PropertyList &properties) { setProperties(properties.toRange()); }

 private:
  Queue queue_;
  PropertyList properties_;

  enum {
    SizeWithoutProperties = sizeof(queue_)
  };

  friend class QueueList;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_QUEUE_H_
