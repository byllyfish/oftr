// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/segmentcache.h"

using namespace ofp;
using ofp::demux::Segment;
using ofp::demux::SegmentCache;

// N.B. has to handle 32-bit wrapping.
bool Segment::lessThan(UInt32 lhs, UInt32 rhs) {
  if (lhs < rhs) {
    return (rhs - lhs < 0x80000000);
  } else if (lhs > rhs) {
    return (lhs - rhs > 0x80000000);
  }
  return false;
}

void SegmentCache::store(UInt32 end, const ByteRange &data, bool final) {
  // Do nothing if new data segment is empty, unless it's the final segment.
  if (data.empty() && !final)
    return;

  UInt32 begin = end - UInt32_narrow_cast(data.size());

  // Search for a segment where we can append the incoming segment [begin,end).
  for (size_t i = 0; i < segments_.size(); ++i) {
    Segment &seg = segments_[i];

    if (begin == seg.end()) {
      if (seg.final()) {
        if (!final || !data.empty()) {
          log::warning("SegmentCache: can't append data to final segment:", data.size());
        }
      } else {
        seg.append(data, final);
        update(i, final);
      }
      goto DONE;
    }

    if (end == seg.begin()) {
      if (final) {
        insert(end, data, i, final);
      } else {
        seg.prepend(data);
      }
      goto DONE;
    }

    if (Segment::lessThan(end, seg.begin())) {
      insert(end, data, i, final);
      goto DONE;
    }

    assert(Segment::lessThan(seg.begin(), end));

    if (Segment::lessThan(begin, seg.end())) {
      // New segment overlaps with existing data.
      log::warning("SegmentCache: data [", begin, ",", end, ") overlaps with [",
                   seg.begin(), ",", seg.end(), ")");
      // FIXME: ignore overlapping data for now...
      goto DONE;
    }

    if (seg.final()) {
      if (!final || !data.empty()) {
        log::warning("SegmentCache: final segment seen already");
      }
      goto DONE;
    }
  }

  append(end, data, final);

DONE:
  assert(checkInvariant());
  log::debug("SegmentCache::store", data.size(), "bytes", toString());
}

const Segment *SegmentCache::current() const {
  return segments_.empty() ? nullptr : &segments_[0];
}

void SegmentCache::consume(size_t len) {
  if (!segments_.empty()) {
    Segment &seg = segments_[0];
    if (len >= seg.size()) {
      assert(len == seg.size());
      segments_.erase(segments_.begin());
    } else {
      seg.consume(len);
    }
  }
  log::debug("SegmentCache::consume", len, "bytes", toString());
}


std::string SegmentCache::toString() const {
  // Examples:  "[segment1)...[segmentN)=size"
  //            "[segment1)...[segmentN)*=size"
  //            "[segment)*"
  // (empty)    ""
  std::ostringstream oss;
  size_t sum = 0;
  for (const auto &seg : segments_) {
    oss << SegmentToString(seg.begin(), seg.end(), seg.final());
    sum += seg.size();
  }
  if (sum > 0) {
    oss << '=' << sum;
  }
  return oss.str();
}

void SegmentCache::append(UInt32 end, const ByteRange &data, bool final) {
  segments_.emplace_back(end, data, final);
}

void SegmentCache::insert(UInt32 end, const ByteRange &data, size_t idx,
                          bool final) {
  segments_.emplace(segments_.begin() + Signed_cast(idx), end, data, final);
}

void SegmentCache::update(size_t idx, bool final) {
  // TODO(bfish): if final is true, we need to clear out the rest of the
  // segments.

  assert(!segments_.empty());

  if (idx < segments_.size() - 1) {
    assert(idx + 1 < segments_.size());

    // Check next segment for overlap.
    auto &seg = segments_[idx];
    auto &next = segments_[idx + 1];
    assert(!seg.final());

    if (seg.end() == next.begin()) {
      seg.append(next.data(), next.final());
      segments_.erase(segments_.begin() + Signed_cast(idx) + 1);
    } else {
      // TODO(bfish): check for overlapping segments.
    }
  }
}

bool SegmentCache::checkInvariant() {
  if (!segments_.empty()) {
    auto *seg = &segments_[0];

    for (size_t i = 1; i < segments_.size(); ++i) {
      auto *next = &segments_[i];

      // Verify that segments are in non-overlapping sorted order.
      if (seg->end() != next->begin() &&
          !Segment::lessThan(seg->end(), next->begin())) {
        log::debug("SegmentCache: segments out of order", toString());
        return false;
      }
      // Verify that only the final segment is empty.
      if (seg->empty() && !seg->final()) {
        log::debug("SegmentCache: empty segment", toString());
        return false;
      }
      // Verify that only the last segment is marked final.
      if (seg->final()) {
        log::debug("SegmentCache: final segment isn't last", toString());
        return false;
      }

      seg = next;
    }
  }

  // Warn if the segment cache contains only one empty final segment.
  if (segments_.size() == 1) {
    const auto &seg = segments_[0];
    if (seg.empty()) {
      log::warning("SegmentCache: exactly one empty segment present!");
    }
  }

  return true;
}

size_t SegmentCache::cacheSize() const {
  size_t sum = 0;
  for (const auto &seg : segments_) {
    sum += seg.size();
  }
  return sum;
}

std::string ofp::demux::SegmentToString(UInt32 begin, UInt32 end, bool final) {
  std::ostringstream oss;
  if (begin != end) {
    oss << "[" << begin << "," << end << ")";
  } else {
    oss << "[" << begin << ")";
  }
  if (final)
    oss << '*';
  return oss.str();
}
