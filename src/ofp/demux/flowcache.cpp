// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/flowcache.h"
#include <iomanip>        // for stats
#include <map>            // for stats
#include <unordered_set>  // for stats

using namespace ofp;
using namespace ofp::demux;

const double kTwoMinuteTimeout = 120.0;

static std::string tcpFlagToString(UInt8 flags);

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

void detail::FlowCacheEntry::reset(const Timestamp &ts, UInt64 sessID) {
  x.clear();
  y.clear();
  lastSeen.clear();
  firstSeen = ts;
  sessionID = sessID;
}

FlowData FlowCache::receive(const Timestamp &ts, const IPv6Endpoint &src,
                            const IPv6Endpoint &dst, UInt32 seq, ByteRange data,
                            UInt8 flags) {
  const UInt8 kInterestingFlags = TCP_SYN | TCP_FIN | TCP_RST;
  const UInt8 kFinalFlags = TCP_FIN | TCP_RST;

  // `seq` is the sequence number from the incoming segment. Internally, we
  // represent all segments [a, b) by the end `b`. Convert seq to `end`.
  UInt32 end;
  if (flags & TCP_SYN) {
    // If the SYN flag is included, `seq` is the initial seqeuence number. A
    // SYN or SYN-ACK packet must not contain any data.
    end = seq + 1;
    if (!data.empty()) {
      log_warning("FlowCache: TCP SYN has unexpected data", data.size(),
                  "bytes:", data);
      return FlowData{0};
    }
  } else {
    end = seq + UInt32_narrow_cast(data.size());
  }

  // Only update cache entry if there is data, or a SYN|FIN|RST flag is present.
  // We drop plain empty ACK segments for example. Specify a session ID of 0,
  // since we're dropping the segment before looking up the session entry.
  if (data.empty() && (flags & kInterestingFlags) == 0) {
    log_debug("TCP ignore empty", tcpFlagToString(flags), src, dst, end);
    return FlowData{0};
  }

  // Is the FIN or RST flag set?
  bool final = (flags & kFinalFlags) != 0;

  bool isX;
  detail::FlowCacheKey key{src, dst, isX};
  detail::FlowCacheEntry &entry = cache_[key];

  if (entry.sessionID == 0) {
    // This is a new entry. Assign a new ID to the session.
    entry.reset(ts, assignSessionID());
  } else if (entry.finished()) {
    // This is a packet for a finished entry. We check for expiry so we don't
    // open a new session just because of a late re-transmit. However, a
    // SYN or SYN-ACK flag means we can skip the timeout.
    if ((flags & TCP_SYN) != 0 || entry.secondsSince(ts) >= kTwoMinuteTimeout) {
      entry.reset(ts, assignSessionID());
    } else {
      log_warning("TCP late segment ignored", entry.sessionID,
                  tcpFlagToString(flags), src, dst, end);
      return FlowData{entry.sessionID};
    }
  } else if ((flags & TCP_SYNACK) == TCP_SYN &&
             entry.secondsSince(ts) >= kTwoMinuteTimeout) {
    // This is a SYN (but not SYN-ACK) for an *unfinished* entry. We open a new
    // session if the connection has been idle for two minutes.
    log_warning("TCP SYN for unfinished entry", entry.sessionID,
                tcpFlagToString(flags), src, dst, end);
    entry.reset(ts, assignSessionID());
  }

  log_debug("TCP segment", entry.sessionID, tcpFlagToString(flags), src, dst,
            entry.secondsSince(ts));

  if (isX) {
    return entry.x.receive(ts, end, data, entry.sessionID, final,
                           &entry.lastSeen);
  }

  return entry.y.receive(ts, end, data, entry.sessionID, final,
                         &entry.lastSeen);
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

detail::FlowCacheEntry *FlowCache::findEntry(const IPv6Endpoint &src,
                                             const IPv6Endpoint &dst) {
  bool isX;
  detail::FlowCacheKey key{src, dst, isX};

  auto iter = cache_.find(key);
  if (iter != cache_.end()) {
    return &iter->second;
  }

  return nullptr;
}

void FlowCache::finish(detail::FlowCallback callback, size_t maxMissingBytes) {
  for (auto &iter : cache_) {
    auto &key = iter.first;
    auto &entry = iter.second;

    entry.x.addMissingData(maxMissingBytes);
    entry.y.addMissingData(maxMissingBytes);

    callback(key.x, key.y, entry.x.latestData(entry.sessionID));
    callback(key.y, key.x, entry.y.latestData(entry.sessionID));
  }
}

UInt64 FlowCache::assignSessionID() {
  if (++assignSessionID_ == 0) {
    return ++assignSessionID_;
  }
  return assignSessionID_;
}

std::string FlowCache::toString() const {
  std::ostringstream oss;
  for (const auto &iter : cache_) {
    auto &key = iter.first;
    auto &entry = iter.second;

    oss << entry.sessionID << ' ' << key.x << "<->" << key.y << ' '
        << entry.x.toString() << '|' << entry.y.toString() << '\n';
  }
  return oss.str();
}

std::string FlowCache::stats() const {
  std::ostringstream oss;
  oss << "FlowCache size=" << cache_.size()
      << " bucket_count=" << cache_.bucket_count()
      << " load_factor=" << cache_.load_factor()
      << " max_load_factor=" << cache_.max_load_factor() << '\n';

  // Make histogram of bucket sizes for the FlowCacheKey.
  std::map<UInt32, UInt32> histogram;
  for (size_t i = 0; i < cache_.bucket_count(); ++i) {
    UInt32 bktSize = UInt32_narrow_cast(cache_.bucket_size(i));
    histogram[bktSize] += 1;
  }

  for (const auto &iter : histogram) {
    oss << std::setw(2) << iter.first << ": " << iter.second << '\n';
  }

  // Hash all the addresses into an unordered set.
  std::unordered_set<IPv6Address> addrs;
  addrs.max_load_factor(0.9f);
  for (const auto &iter : cache_) {
    addrs.insert(iter.first.x.address());
    addrs.insert(iter.first.y.address());
  }
  oss << "IPv6Address size=" << addrs.size()
      << " bucket_count=" << addrs.bucket_count()
      << " load_factor=" << addrs.load_factor()
      << " max_load_factor=" << addrs.max_load_factor() << '\n';

  // Make histogram of bucket sizes for the address set.
  histogram.clear();
  for (size_t i = 0; i < addrs.bucket_count(); ++i) {
    UInt32 bktSize = UInt32_narrow_cast(addrs.bucket_size(i));
    histogram[bktSize] += 1;
  }

  for (const auto &iter : histogram) {
    oss << std::setw(2) << iter.first << ": " << iter.second << '\n';
  }

  return oss.str();
}

static std::string tcpFlagToString(UInt8 flags) {
  std::string result;

  if (flags & TCP_SYN) {
    result += 'S';
  }
  if (flags & TCP_FIN) {
    result += 'F';
  }
  if (flags & TCP_RST) {
    result += 'R';
  }
  if (flags & TCP_ACK) {
    result += 'A';
  }
  return result;
}
