// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/queuerange.h"
#include "ofp/writable.h"

using namespace ofp;

size_t QueueRange::writeSize(Writable *channel) {
  if (channel->version() > OFP_VERSION_4) {
    return size();
  }

  if (channel->version() > OFP_VERSION_2) {
    // Determine size of queues when packed together without padding.
    size_t size = 0;
    for (auto &item : *this) {
      size += queueSize(item);
    }
    return size;
  }

  // Versions 1 and 2 use the QueueV1 structure instead. This structure is
  // 8 bytes smaller than the Queue structure when encoded. However, a queue
  // with an empty property list adds the OFPQT_NONE property (8 bytes).
  size_t size = 0;
  for (auto &item : *this) {
    size_t queueLen = item.len_;
    assert(queueLen >= 16);
    size += (queueLen == 16 ? 16 : queueLen - 8);
  }

  return size;
}

void QueueRange::write(Writable *channel) {
  if (channel->version() > OFP_VERSION_4) {
    channel->write(data(), size());

  } else if (channel->version() > OFP_VERSION_2) {
    // We need to pack the queues together. They are not necessarily aligned
    // to 8-byte boundaries (as they are stored internally).

    for (auto &item : *this) {
      Queue slice;
      std::memcpy(&slice, &item, sizeof(slice));
      slice.len_ = UInt16_narrow_cast(queueSize(item));
      channel->write(&slice, sizeof(slice));
      for (auto &prop : item.properties()) {
        channel->write(&prop, prop.size());
      }
    }

  } else {
    // Version 1 uses the QueueV1 structure instead.
    for (auto &item : *this) {
      deprecated::QueueV1Builder queueV1{item};
      queueV1.write(channel);
    }
  }
}

size_t QueueRange::queueSize(const Queue &queue) {
  size_t size = sizeof(Queue);
  for (auto &prop : queue.properties()) {
    size += prop.size();
  }
  return size;
}
