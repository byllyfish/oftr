// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_QUEUERANGE_H_
#define OFP_QUEUERANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/queue.h"

namespace ofp {

using QueueIterator = ProtocolIterator<Queue>;
using QueueRange = ProtocolRange<QueueIterator>;

}  // namespace ofp

#endif  // OFP_QUEUERANGE_H_
