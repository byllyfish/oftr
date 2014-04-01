#ifndef OFP_QUEUERANGE_H_
#define OFP_QUEUERANGE_H_

#include "ofp/protocoliterable.h"

namespace ofp {

using QueueIterator = ProtocolIterator<Queue>;
using QueueRange = ProtocolIterable<Queue, QueueIterator>;

}  // namespace ofp

#endif // OFP_QUEUERANGE_H_
