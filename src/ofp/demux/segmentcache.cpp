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

void SegmentCache::store(UInt32 end, const ByteRange &data) {
  // Do nothing if new data segment is empty.
  if (data.empty())
    return;

  UInt32 begin = end - UInt32_narrow_cast(data.size());

  for (size_t i = 0; i < segments_.size(); ++i) {
    Segment &seg = segments_[i];

    if (begin == seg.end()) {
      seg.append(data);
      update(i);
      goto DONE;
    }

    if (end == seg.begin()) {
      seg.prepend(data);
      goto DONE;
    }

    if (Segment::lessThan(end, seg.begin())) {
      insert(end, data, i);
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
  }

  append(end, data);

DONE:
  assert(checkInvariant());
  log::debug("SegmentCache::store", data.size(), "bytes", toString());
}

const Segment *SegmentCache::current() const {
  if (segments_.empty())
    return nullptr;
  return &segments_[0];
}

void SegmentCache::consume(size_t len) {
  if (!segments_.empty()) {
    Segment &seg = segments_[0];
    if (len >= seg.size()) {
      segments_.erase(segments_.begin());
    } else {
      seg.consume(len);
    }
  }
  log::debug("SegmentCache::consume", len, "bytes", toString());
}

std::string SegmentCache::toString() const {
  std::ostringstream oss;
  for (auto &seg : segments_) {
    oss << " [" << seg.begin() << "," << seg.end() << ")";
  }
  return oss.str();
}

void SegmentCache::append(UInt32 end, const ByteRange &data) {
  segments_.emplace_back(end, data);
}

void SegmentCache::insert(UInt32 end, const ByteRange &data, size_t idx) {
  segments_.emplace(segments_.begin() + idx, end, data);
}

void SegmentCache::update(size_t idx) {
  if (idx < segments_.size() - 1) {
    // Check next segment for overlap.
    auto &seg = segments_[idx];
    auto &next = segments_[idx + 1];
    if (seg.end() == next.begin()) {
      seg.append(next.data());
      segments_.erase(segments_.begin() + idx + 1);
    } else {
      // TODO: check for overlapping segments.
    }
  }
}

bool SegmentCache::checkInvariant() {
  if (!segments_.empty()) {
    auto *seg = &segments_[0];
    for (size_t i = 1; i < segments_.size(); ++i) {
      auto *next = &segments_[i];
      if (seg->end() != next->begin() &&
          !Segment::lessThan(seg->end(), next->begin()))
        return false;
      seg = next;
    }
  }

  return true;
}
