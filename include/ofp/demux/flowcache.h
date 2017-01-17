// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_FLOWCACHE_H_
#define OFP_DEMUX_FLOWCACHE_H_

#include <unordered_map>
#include "ofp/demux/flowstate.h"
#include "ofp/ipv6endpoint.h"

namespace ofp {
namespace demux {
namespace detail {

// Key used in FlowCache. We use a "biflow" storage model (x, y) where
// x < y.  For example, if src is "127.0.0.1:80" and dst is "127.0.0.1:79" then
// x = "127.0.0.1:79" and y = "127.0.0.1:80". The corresponding "flow state" is
// stored in a FlowCacheEntry.

struct FlowCacheKey {
  IPv6Endpoint x;
  IPv6Endpoint y;

  FlowCacheKey(const IPv6Endpoint &src, const IPv6Endpoint &dst, bool &srcIsX);

  bool operator==(const FlowCacheKey &key) const {
    return x == key.x && y == key.y;
  }
};

static_assert(sizeof(FlowCacheKey) == 36, "Unexpected size.");

// Entry used in FlowCache.
struct FlowCacheEntry {
  UInt64 sessionID = 0;
  FlowState x;
  FlowState y;
  Timestamp firstSeen;
  Timestamp lastSeen;

  double secondsSince(const Timestamp &ts) const {
    return ts.secondsSince(lastSeen);
  }
  bool finished() const { return x.finished() && y.finished(); }
  void reset(const Timestamp &ts, UInt64 sessID);
};

using FlowMap = std::unordered_map<FlowCacheKey, FlowCacheEntry>;
using FlowCallback = std::function<void(
    const IPv6Endpoint &, const IPv6Endpoint &dst, const FlowData &)>;

}  // namespace detail
}  // namespace demux
}  // namespace ofp

namespace std {

template <>
struct hash<ofp::demux::detail::FlowCacheKey> {
  size_t operator()(const ofp::demux::detail::FlowCacheKey &key) const {
    return ofp::hash::MurmurHash32(&key);
  }
};

}  // namespace std

namespace ofp {
namespace demux {

enum TCPControlBits : UInt8 {
  TCP_URG = (1 << 5),
  TCP_ACK = (1 << 4),
  TCP_PSH = (1 << 3),
  TCP_RST = (1 << 2),
  TCP_SYN = (1 << 1),
  TCP_FIN = (1 << 0),

  TCP_SYNACK = (TCP_SYN | TCP_ACK)
};

/// Usage:
///
///    FlowCache cache;
///
///    FlowData data = cache.receive(ts, src, dst, seq, data, flags);
///    if (data.size() > 0) {
///      process(ts, src, dst, data.data(), data.size());
///      data.consume(data.size());
///    }
///
///    ...
///
///    cache.finish([](const IPv6Endpoint &src, const IPv6Endpoint &dst, const
///    FlowData &flow) {
//       process(src, dst, data.data(), data.size());
//     });
///
class FlowCache {
 public:
  FlowCache() {
    cache_.max_load_factor(0.9f);
    cache_.reserve(100);
  }

  // Submit data from a tcp segment to update the cache.
  FlowData receive(const Timestamp &ts, const IPv6Endpoint &src,
                   const IPv6Endpoint &dst, UInt32 seq, ByteRange data,
                   UInt8 flags = 0);

  size_t size() const { return cache_.size(); }
  FlowState *lookup(const IPv6Endpoint &src, const IPv6Endpoint &dst);
  detail::FlowCacheEntry *findEntry(const IPv6Endpoint &src,
                                    const IPv6Endpoint &dst);

  // Call a function to process remaining data in the cache.
  void finish(const detail::FlowCallback &callback, size_t maxMissingBytes = 0);

  // Erase all data from the cache.
  void clear() { cache_.clear(); }

  // Describe the contents of the cache (for debugging).
  std::string toString() const;
  std::string stats() const;

 private:
  detail::FlowMap cache_;
  UInt64 assignSessionID_ = 0;

  UInt64 assignSessionID();
};

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_FLOWCACHE_H_
