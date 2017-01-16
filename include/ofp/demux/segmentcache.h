// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_SEGMENTCACHE_H_
#define OFP_DEMUX_SEGMENTCACHE_H_

#include <vector>
#include "ofp/bytelist.h"

namespace ofp {
namespace demux {

/// Describe segment as half-open interval. Final segments have a '*' suffix.
/// e.g. "[12345,12346)" or "[23456,23457)*"
/// Empty segments use only one sequence number, e.g. "[34567)"
std::string SegmentToString(UInt32 begin, UInt32 end, bool final);

OFP_BEGIN_IGNORE_PADDING

class Segment {
 public:
  Segment(UInt32 end, const ByteRange &data, bool final)
      : end_{end}, final_{final}, data_{data} {}

  UInt32 begin() const { return end_ - UInt32_narrow_cast(data_.size()); }
  UInt32 end() const { return end_; }
  size_t size() const { return data_.size(); }
  bool empty() const { return data_.empty(); }
  bool final() const { return final_; }
  ByteRange data() const { return data_.toRange(); }
  ByteRange data(size_t overlap) const {
    return SafeByteRange(data_.data(), data_.size(), overlap);
  }

  void append(const ByteRange &data, bool final) {
    assert(!final_ || final);
    data_.add(data.data(), data.size());
    end_ += data.size();
    final_ = final;
  }

  void prepend(const ByteRange &data) {
    data_.insert(data_.begin(), data.data(), data.size());
  }

  void consume(size_t len) {
    assert(len <= data_.size());
    data_.remove(data_.begin(), len);
  }

  static bool lessThan(UInt32 lhs, UInt32 rhs);

  std::string toString() const {
    return SegmentToString(begin(), end(), final());
  }

 private:
  UInt32 end_ = 0;
  bool final_ = false;  // is this the last segment?
  ByteList data_;
};

OFP_END_IGNORE_PADDING

// Stores TCP segments that need to be reassembled.
class SegmentCache {
 public:
  // Store segment in the cache. If possible, combine this segment with
  // existing segments to fill in gaps.
  void store(UInt32 end, const ByteRange &data, bool final = false);

  // Return ptr to current segment. Return nullptr if there is no segment.
  const Segment *current() const;

  // Drain bytes from the current segment. When all bytes are drained, the
  // current segment is removed.
  void consume(size_t len);

  // Return number of segments.
  size_t segmentCount() const { return segments_.size(); }

  // Return true if cache is empty.
  bool empty() const { return segments_.empty(); }

  // Return description of cached segments for debugging.
  std::string toString() const;

  // Erase all data.
  void clear() { segments_.clear(); }

  // Return total number of bytes cached.
  size_t cacheSize() const;

  // Fill in gaps between segments where the gap <= maxMissingBytes.
  void addMissingData(UInt32 end, size_t maxMissingBytes);

 private:
  std::vector<Segment> segments_;

  void append(UInt32 end, const ByteRange &data, bool final);
  void insert(UInt32 end, const ByteRange &data, size_t idx, bool final);
  void update(size_t idx, bool final);

  bool checkInvariant();
};

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_SEGMENTCACHE_H_
