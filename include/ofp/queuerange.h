// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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

 private:
  size_t queueSize(const Queue &queue);
};

namespace deprecated {

using QueueV1Iterator =
    ProtocolIterator<QueueV1, ProtocolIteratorType::QueueV1>;
using QueueV1Range = ProtocolRange<QueueV1Iterator>;

}  // namespace deprecated

}  // namespace ofp

#endif  // OFP_QUEUERANGE_H_
