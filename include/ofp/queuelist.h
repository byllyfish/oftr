#ifndef OFP_QUEUELIST_H_
#define OFP_QUEUELIST_H_

#include "ofp/queuerange.h"

namespace ofp {

class QueueList : public ProtocolList<QueueRange> {
public:
    void add(const QueueBuilder &queue) {
        const PropertyList &props = queue.properties_;
        buf_.add(&queue, QueueBuilder::SizeWithoutProperties);
        buf_.add(props.data(), props.size());
    }
};

}  // namespace ofp

#endif // OFP_QUEUELIST_H_
