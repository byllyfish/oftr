// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/flowcache.h"

using namespace ofp;
using namespace ofp::demux;

detail::FlowCacheKey::FlowCacheKey(const IPv6Endpoint &src,
                                   const IPv6Endpoint &dst, bool &srcIsX) {
  if (src <= dst) {
    x = src;
    y = dst;
    srcIsX = true;
  } else {
    x = dst;
    y = src;
    srcIsX = false;
  }
}

FlowData FlowCache::receive(const Timestamp &ts, const IPv6Endpoint &src,
                            const IPv6Endpoint &dst, UInt32 seq, ByteRange data,
                            UInt8 flags) {
  // Only update cache entry if there is data, or a SYN|FIN|RST flag is present.
  const UInt8 kInterestingFlags = TCP_SYN | TCP_FIN | TCP_RST;
  const UInt8 kFinalFlags = TCP_FIN | TCP_RST;

  if (data.empty() && (flags & kInterestingFlags) == 0)
    return FlowData{0};

  bool final = (flags & kFinalFlags) != 0;

  bool isX;
  detail::FlowCacheKey key{src, dst, isX};
  detail::FlowCacheEntry &entry = cache_[key];

  if (entry.sessionID == 0) {
    entry.sessionID = assignSessionID();
  }

  if (isX) {
    return entry.x.receive(ts, seq, data, entry.sessionID, final);
  } else {
    return entry.y.receive(ts, seq, data, entry.sessionID, final);
  }
}

FlowState *FlowCache::lookup(const IPv6Endpoint &src, const IPv6Endpoint &dst) {
  bool isX;
  detail::FlowCacheKey key{src, dst, isX};

  auto iter = cache_.find(key);
  if (iter != cache_.end()) {
    auto &entry = iter->second;
    return isX ? &entry.x : &entry.y;
  }

  return nullptr;
}

void FlowCache::finish(FlowCallback callback) {
  for (auto &iter : cache_) {
    auto &key = iter.first;
    auto &entry = iter.second;

    callback(key.x, key.y, entry.x.latestData(entry.sessionID));
    callback(key.y, key.x, entry.y.latestData(entry.sessionID));
  }
}

UInt64 FlowCache::assignSessionID() {
  if (++sessionID_ == 0) {
    return ++sessionID_;
  }
  return sessionID_;
}
