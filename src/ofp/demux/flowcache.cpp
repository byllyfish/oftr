// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/flowcache.h"

using namespace ofp;
using namespace ofp::demux;

const double kTwoMinuteTimeout = 120.0;
const double kFiveMinuteTimeout = 300.0;

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

bool detail::FlowCacheEntry::expired(const Timestamp &ts, double seconds) const {
  Timestamp last = std::max(x.lastSeen(), y.lastSeen());
  return ts.secondsSince(last) >= seconds;
}

/// An entry is `finished` when it's closed in both directions.
bool detail::FlowCacheEntry::finished() const {
  return x.finished() && y.finished();
}

double detail::FlowCacheEntry::timeDelta(const Timestamp &ts) const {
  Timestamp last = std::max(x.lastSeen(), y.lastSeen());
  return ts.secondsSince(last);
}

void detail::FlowCacheEntry::clear(UInt64 sessID) {
  sessionID = sessID;
  x.clear();
  y.clear();
}

FlowData FlowCache::receive(const Timestamp &ts, const IPv6Endpoint &src,
                            const IPv6Endpoint &dst, UInt32 seq, ByteRange data,
                            UInt8 flags) {
  // `seq` is the sequence number from the incoming segment. Internally, we
  // represent all segments [a, b) by the end `b`. Convert seq to `end`.
  UInt32 end;
  if (flags & TCP_SYN) {
    // If the SYN flag is included, `seq` is the initial seqeuence number.
    end = seq + 1;
    if (data.size() > 0) {
      log::warning("FlowCache: TCP SYN has unexpected data", data.size());
      return FlowData{0};
    }
  } else {
    end = seq + UInt32_narrow_cast(data.size());
  }

  // Only update cache entry if there is data, or a SYN|FIN|RST flag is present.
  // We drop plain empty ACK segments for example.
  const UInt8 kInterestingFlags = TCP_SYN | TCP_FIN | TCP_RST;
  const UInt8 kFinalFlags = TCP_FIN | TCP_RST;

  if (data.empty() && (flags & kInterestingFlags) == 0) {
    log::debug("TCP ignore empty", tcpFlagToString(flags), src, dst, end);
    return FlowData{0};
  }

  bool final = (flags & kFinalFlags) != 0;

  bool isX;
  detail::FlowCacheKey key{src, dst, isX};
  detail::FlowCacheEntry &entry = cache_[key];

  if (entry.sessionID == 0) {
    // This is a new entry. Assign a new ID to the session.
    entry.sessionID = assignSessionID();
  } else if (entry.finished()) {
    // This is a packet for a finished entry. We check for expiry so we don't 
    // open a new session just because of a late re-transmit. However, a SYN
    // flag means we can skip the timeout.
    if ((flags & TCP_SYN) != 0 || entry.expired(ts, kTwoMinuteTimeout)) {
      entry.clear(assignSessionID());
    } else {
      log::warning("TCP late segment ignored", entry.sessionID, tcpFlagToString(flags), src, dst, end);
      return FlowData{0};
    }
  } else if ((flags & TCP_SYN) != 0 && entry.expired(ts, kFiveMinuteTimeout)) {
    // This is a SYN or SYN-ACK for an unfinished entry. We open a new session
    // if the connection has been silent for five minutes.
    log::warning("TCP SYN for unfinished entry", entry.sessionID, tcpFlagToString(flags), src, dst, end);
    entry.clear(assignSessionID());
  }

  log::debug("TCP segment", entry.sessionID, tcpFlagToString(flags), src, dst, entry.timeDelta(ts));

  if (isX) {
    return entry.x.receive(ts, end, data, entry.sessionID, final);
  } else {
    return entry.y.receive(ts, end, data, entry.sessionID, final);
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

void FlowCache::finish(detail::FlowCallback callback) {
  for (auto &iter : cache_) {
    auto &key = iter.first;
    auto &entry = iter.second;

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

    oss << entry.sessionID << ' ' << key.x << "<->" << key.y << ' ' << entry.x.toString() << '|' << entry.y.toString() << '\n';
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
