// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_QUEUERANGE_H_
#define OFP_QUEUERANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/queue.h"

namespace ofp {

class Writable;

using QueueIterator = ProtocolIterator<Queue, ProtocolIteratorType::Queue>;

class QueueRange : public ProtocolRange<QueueIterator> {
  using Inherited = ProtocolRange<QueueIterator>;

 public:
  using Inherited::Inherited;

  /// \returns Size of queue list when written to channel using the specified
  /// protocol version.
  size_t writeSize(Writable *channel);

  /// \brief Writes queue list to the channel using the specified protocol
  /// version.
  void write(Writable *channel);
};

namespace deprecated {

using QueueV1Iterator =
    ProtocolIterator<QueueV1, ProtocolIteratorType::QueueV1>;
using QueueV1Range = ProtocolRange<QueueV1Iterator>;

}  // namespace deprecated

}  // namespace ofp

#endif  // OFP_QUEUERANGE_H_
