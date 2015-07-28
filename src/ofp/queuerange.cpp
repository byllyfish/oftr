#include "ofp/queuerange.h"
#include "ofp/writable.h"

using namespace ofp;


size_t QueueRange::writeSize(Writable *channel) {
    if (channel->version() > OFP_VERSION_1) {
        return size();
    }
    
    // Version 1 uses the QueueV1 structure instead. This structure is 8ytes
    // smaller than the Queue structure when encoded. However, a queue with 
    // an empty property list adds the OFPQT_NONE property (8 bytes).
    size_t size = 0;
    for (auto &item : *this) {
        size_t queueLen = item.len_;
        assert(queueLen >= 16);
        size += (queueLen == 16 ? 16 : queueLen - 8);
    }

    return size;
}


void QueueRange::write(Writable *channel) {
    if (channel->version() > OFP_VERSION_1) {
        channel->write(data(), size());

    } else {
        // Version 1 uses the QueueV1 structure instead.
        for (auto &item : *this) {
            deprecated::QueueV1Builder queueV1{item};
            queueV1.write(channel);
        }
    }
}
