#ifndef OFP_DEMUX_FLOWCACHE_H_
#define OFP_DEMUX_FLOWCACHE_H_

#include "ofp/ipv6endpoint.h"
#include "ofp/demux/flowstate.h"
#include <unordered_map>

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
  bool operator==(const FlowCacheKey &key) const { return x == key.x && y == key.y; }
};

// Entry used in FlowCache.
struct FlowCacheEntry {
  UInt64 sessionID = 0;
  FlowState x;
  FlowState y;
};

}  // namespace detail
}  // namespace demux
}  // namespace ofp

namespace std {

template <>
struct hash<ofp::demux::detail::FlowCacheKey> {
  size_t operator()(const ofp::demux::detail::FlowCacheKey &key) const {
    size_t result = 0;
    ofp::HashCombine(result, std::hash<ofp::IPv6Endpoint>{}(key.x));
    ofp::HashCombine(result, std::hash<ofp::IPv6Endpoint>{}(key.y));
    return result;
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
  TCP_FIN = (1 << 0)
};

/// Usage:
/// 
///    FlowCache cache;
///    
///    FlowData data = cache.receive(ts, src, dst, seq, data, flags);
///    if (data.size() > 0) {
///       process(ts, src, dst, data.data(), data.size());
///       data.consume(data.size());
///    }
///    
class FlowCache {
public:
  // Submit data from a tcp segment to update the cache.
  FlowData receive(const Timestamp &ts, const IPv6Endpoint &src, const IPv6Endpoint &dst, UInt32 seq, ByteRange data, UInt8 flags = 0);

  size_t size() const { return cache_.size(); }
  FlowState *lookup(const IPv6Endpoint &src, const IPv6Endpoint &dst);

private:
  std::unordered_map<detail::FlowCacheKey, detail::FlowCacheEntry> cache_;
  UInt64 sessionID_ = 0;

  UInt64 assignSessionID() { 
    if (++sessionID_ == 0) {
      return ++sessionID_;
    }
    return sessionID_;
  }
};

}  // namespace demux
}  // namespace ofp

#endif // OFP_DEMUX_FLOWCACHE_H_
